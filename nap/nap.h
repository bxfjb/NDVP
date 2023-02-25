//
// Created by Lenovo on 2023/2/24.
//

#ifndef NETWORKSYSTEM_NAP_H
#define NETWORKSYSTEM_NAP_H

#include <thread>
#include <memory>

#include "../ndvp/ndvp.h"

class Server {
public:
    Server(std::shared_ptr<Router> router): m_router(router) {
        RecvWork();
        fprintf(stdout, "NAP Server running...\n");
    }

    ~Server() {
        m_recv_thread.join();
    }
    void RecvWork();
    int RecvPacket();

    int ParseRequest(const char* data, struct RequestPacket* request);
    int ParsePayload(const char* data, struct PayloadPacket* payload);

    int SendResponse(struct sockaddr* client_addr, uint16_t stream_number);

    int Forward(char* data, uint16_t out_label, std::string next_ip, int size);

private:
    std::shared_ptr<Router> m_router;
    //Router *m_router;
    std::thread m_recv_thread;
    int info_len[7] = {0,0,0,4,4,8,12};
};

class Client {
public:
    Client(std::string server_addr): m_server_addr(server_addr) {
        read_req_data();
        read_payload_data();
        sleep(5);
        SendRequest();
    }
    ~Client() {
        if (m_request) {
            free(m_request);
            m_request = nullptr;
        }
    }

    int SendRequest();

    int SendPayload();

private:
    void read_req_data();
    void read_payload_data();
    std::string m_server_addr;
    int m_request_len;
    std::string m_payload{};
    int info_len[7] = {0,0,0,4,4,8,12};
    RequestPacket* m_request{nullptr};
};

#endif //NETWORKSYSTEM_NAP_H
