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
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>

class Router;

struct Attribute{
    uint32_t delay;
    uint32_t computing_rate;
    uint32_t bandwidth;
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

class Router {
    #define PORT_BEGIN 12344
public:
    Router(uint16_t router_id, uint32_t AS_number, const char *local_ip):
        m_router_id(router_id), m_AS_number(AS_number), m_local_ip(local_ip) {
            m_port = PORT_BEGIN + m_router_id;
            RecvWork();
            printf("R%u RecvWork() Start\n", m_router_id);
        }

    ~Router() {
        m_recv_thread.join();
        printf("R%u End\n", m_router_id);
    }
    
    uint16_t m_router_id;
    uint32_t m_AS_number;
    const char* m_local_ip;
    uint16_t m_port;
    std::unordered_map<uint16_t, const char*> m_peer_table; // <router-id,ip>
    std::vector<Path> m_adj_out;

    std::thread m_recv_thread;

    int SendHello(const char* peer_addr, uint16_t peer_port);
    int SendAdvertise();

    int RecvPacket(const char* local_ip);

    int ParseHello(const char* data, struct HelloPacket* hello);
    int ParseAdvertise(const char* data, struct AdvertisePacket* advertise);

    void RecvWork();

};





#endif //NETWORKSYSTEM_NDVP_H
