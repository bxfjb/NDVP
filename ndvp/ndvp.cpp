//
// Created by Lenovo on 2023/2/21.
//

#include "ndvp.h"
#include "util.h"
#include <string.h>
#include <iostream>


int Router::SendHello(const char* peer_addr, uint16_t peer_port) {
    int ret, socket;
    struct sockaddr_in addr;

    struct HelloPacket hello;
    hello.router_id = htons(m_router_id);
    hello.AS_number = htonl(m_AS_number);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(peer_port);
    addr.sin_addr.s_addr = inet_addr(peer_addr);
    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    ret = sendto(socket, (char*)&hello, sizeof(hello), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 1) {
        perror("sendto");
        return -1;
    }

    close(socket);

    return 0;
}

int Router::SendAdvertise() {
    int ret, socket;
    struct sockaddr_in addr;

    int adver_len = sizeof(struct AdvertisePacket)+sizeof(struct PathInPacket)*m_adj_out.size();
    auto adver = (struct AdvertisePacket *)malloc(adver_len);
    memset(adver, 0, adver_len);
    adver->type = 2;
    adver->checksum = 0;
    adver->router_id = htons(this->m_router_id);
    adver->path_num = htonl((uint32_t)m_adj_out.size());
    
    for (int i = 0;i < m_adj_out.size();++i) {
        adver->paths[i].sid = htons(m_adj_out[i].sid);
        adver->paths[i].in_label = htons(m_adj_out[i].in_label);
        adver->paths[i].attr.delay = htonl(m_adj_out[i].attr.delay);
        adver->paths[i].attr.bandwidth = htonl(m_adj_out[i].attr.bandwidth);
        adver->paths[i].attr.computing_rate = htonl(m_adj_out[i].attr.computing_rate);
    }

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    for (auto &i:m_peer_table) {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(i.second);
        addr.sin_port = htons(12345);
        
        ret = sendto(socket, (char*)adver, adver_len, 0, (struct sockaddr*)&addr, sizeof(addr));
        if (ret < 1) {
            perror("sendto");
            return -1;
        }
    }

    free(adver);
    close(socket);

    return 0;
}

int Router::RecvPacket(const char* local_ip) {
    int ret, socket;
    struct sockaddr_in addr;

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(local_ip);
    addr.sin_port = htons(m_port);

    if (bind(socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    char buff[1024];

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        ssize_t s = recvfrom(socket, buff, sizeof(buff)-1, 0, (struct sockaddr*)&client_addr, &len);
        if (s > 0) {
            int type = htonl(buff[0]);
            if (type == 1) {
                struct HelloPacket hello;
                if (ParseHello(buff, &hello) >= 0) {
                    if (m_peer_table.count(hello.router_id) == 0) {
                        const char* peer_addr = inet_ntoa(client_addr.sin_addr);
                        m_peer_table[hello.router_id] = peer_addr;
                        SendHello(peer_addr, PORT_BEGIN + hello.router_id);
                    }
                }
            } else if (type == 2) {
                struct AdvertisePacket advertise;
                ParseAdvertise(buff, &advertise);
            } else {
                perror("Unknown packet type");
                return -1;
            }
        }
    }

    close(socket);
    return 0;
}

int Router::ParseHello(const char* data, struct HelloPacket* hello) {
    hello->router_id = ntohs((uint16_t)data[2]);
    hello->AS_number = ntohl((uint32_t)data[4]);
    if (hello->AS_number != m_AS_number) {
        return -1;
    }
    return 0;
}

int Router::ParseAdvertise(const char* data, struct AdvertisePacket* advertise) {
    return 0;
}

void Router::RecvWork() {
    m_recv_thread = std::thread(&Router::RecvPacket, this, m_local_ip);
}