//
// Created by Lenovo on 2023/2/21.
//

#ifndef NETWORKSYSTEM_NDVP_H
#define NETWORKSYSTEM_NDVP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <mutex>

class Router;

#define NDVP_PORT 12345
#define NAP_PORT 12346
#define FIRST_HELLO 5

struct Attribute{
    uint32_t delay;
    uint32_t bandwidth;
    uint32_t computing_rate;
    Attribute operator+(Attribute &a1) {
        Attribute a2;
        a2.delay = delay + a1.delay;
        a2.bandwidth = std::min(bandwidth, a1.bandwidth);
        a2.computing_rate = std::max(computing_rate, a1.computing_rate);
        return a2;
    }

    bool operator==(Attribute &a1) {
        return delay == a1.delay && bandwidth == a1.bandwidth && computing_rate == a1.computing_rate;
    }
};

struct Path{
    int index;
    uint16_t next_hop_id;
    int16_t in_label, out_label;
    uint16_t sid;
    Attribute attr;
};

struct PathInPacket {
    uint16_t sid;
    uint16_t in_label;
    Attribute attr;
};

struct HelloPacket{
    uint8_t type = 1;
    uint8_t checksum = 0;
    uint16_t router_id{};
    uint32_t AS_number{};
};

struct AdvertisePacket {
    uint8_t type = 2;
    uint8_t checksum = 0;
    uint16_t router_id{};
    uint32_t path_num{};
    PathInPacket paths[];
};

struct PathInvalidInPacket {
    uint16_t sid;
    uint16_t in_label;
};

struct AdvertiseInvalidPacket {
    uint8_t type = 3;
    uint8_t checksum = 0;
    uint16_t router_id{};
    uint32_t path_num{};
    PathInvalidInPacket paths[];
};

struct RequestPacket {
    uint8_t type = 1;
    uint8_t checksum = 0;
    uint16_t sid;
    uint16_t criteria;
    uint16_t stream_num;
    uint32_t info[];
};

struct ResponsePacket {
    uint8_t type = 2;
    uint8_t checksum = 0;
    uint16_t stream_num;
};

struct PayloadPacket {
    uint8_t type = 3;
    uint8_t checksum = 0;
    uint16_t content_length;
    uint16_t label;
    uint16_t stream_number;
    char data[];
};

class Router {
    enum criteria {
        unknown,
        SW = 1,
        WS,
        K_Q,
        W_S,
        N_K_Q,
        N_W_S,
    };

    std::string cri_str[7] = {
        "unknown",
        "SW",
        "WS",
        "K_Q",
        "W_S",
        "N_K_Q",
        "N_W_S",
    };

    std::string proto_str[3] = {
        "NDVP",
        "PDP",
        "EIGRP",
    };
public:
    Router(uint16_t router_id, uint32_t AS_number, std::vector<std::pair<const char*,const char*>>&local_ip, int proto_type = 0):
        m_router_id(router_id), m_AS_number(AS_number), m_local_ip(local_ip), m_proto_type(proto_type) {
            m_port = NDVP_PORT;
            read_edge_data();
            RecvWork();
            
            sleep(FIRST_HELLO);
            SendHello();
            sleep(5);
            read_com_data();
        }

    ~Router() {
        for (auto &t:m_recv_threads)
            t.join();
        m_adj_in_thread.join();
        m_adj_ini_thread.join();
        m_adj_out_thread.join();
        m_adj_outi_thread.join();
        fprintf(stdout, "R[%u] End\n", m_router_id);
    }
    
    uint16_t m_router_id;
    uint32_t m_AS_number;
    std::vector<std::pair<const char*,const char*>> m_local_ip;
    uint16_t m_port;
    std::unordered_map<uint16_t, std::string> m_peer_table; // <router-id,ip>
    std::unordered_map<uint16_t, Attribute> m_edge_table;   // <router-id,edge>
    std::vector<Path> m_adj_out;           // only use data so pointer
    std::vector<Path> m_adj_out_invalid;    // when path is added in here, the origin object is free, we need create a new path, so data
    std::vector<std::pair<PathInPacket,uint16_t>> m_adj_in; // second element is sender's router-id
    std::vector<std::pair<PathInvalidInPacket,uint16_t>> m_adj_in_invalid;
    std::unordered_map<uint16_t, std::vector<Path>> m_rib;
    std::unordered_map<uint16_t, Path> m_fib; // <in_label, path>
    std::unordered_map<uint16_t, Path> m_buff_fib; // <stream_number, path>

    std::vector<std::thread> m_recv_threads;
    std::thread m_adj_in_thread;
    std::thread m_adj_out_thread;
    std::thread m_adj_ini_thread;
    std::thread m_adj_outi_thread;
    std::thread m_NAP_thread;

    int m_proto_type = 0;  // 0--NVDP 1--PDP 2--EIGRP
    int m_next_label = 1;
    int m_path_index = 1;

    std::mutex m_adj_in_mutex;
    std::mutex m_adj_ini_mutex;
    std::mutex m_adj_out_mutex;
    std::mutex m_adj_outi_mutex;
    std::mutex m_peer_table_mutex;

    int SendHello();                            // broadcast Hello packet
    int SendAdvertise();
    int SendAdvertiseInvalid();

    int ResponseHello(const char* peer_addr);   // unicast Hello packet to peer 

    int RecvPacket(const char* local_ip);

    int ParseHello(const char* data, struct HelloPacket* hello);
    int ParseAdvertise(const char* data, struct AdvertisePacket* advertise);
    int ParseAdvertiseInvalid(const char* data, struct AdvertiseInvalidPacket* advertise);

    void DealAdjIn();
    void DealAdjInInvalid();

    void RecvWork();

    void CheckAdjOut();
    void CheckAdjIn();
    void CheckAdjOutInvalid();
    void CheckAdjInInvalid();

    void ShowInfo();

    void Shell();       // Interactive shell, init request or delete

    void CalculateBestPath(RequestPacket* request);

    std::pair<uint16_t, std::string> GetLabelByNumber(uint16_t stream_number);
    std::pair<uint16_t, std::string> GetLabelByLabel(uint16_t label);

private:
    void init();        // Read in edge data and com data if exists
    void read_com_data();
    void read_edge_data();

    Path format_trans(PathInPacket &pip, uint16_t rid);

    bool better_ndvp(Attribute &a1, Attribute &a2);
    bool better_pdp(Attribute &a1, Attribute &a2);
    bool better_eigrp(Attribute &a1, Attribute &a2);

    void add_path(std::vector<Path> &path_set, Path &path);
    void delete_path(Path path);

    bool better(Attribute a1, Attribute a2, uint16_t criteria, uint32_t k = 0, uint32_t k1 = 0, uint32_t w = 0);
};





#endif //NETWORKSYSTEM_NDVP_H
