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

int Router::SendAdvertise(uint16_t peer_port) {
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
        adver->paths[i].sid = htons(m_adj_out[i]->sid);
        adver->paths[i].in_label = htons(m_adj_out[i]->in_label);
        adver->paths[i].attr.delay = htonl(m_adj_out[i]->attr.delay);
        adver->paths[i].attr.bandwidth = htonl(m_adj_out[i]->attr.bandwidth);
        adver->paths[i].attr.computing_rate = htonl(m_adj_out[i]->attr.computing_rate);
    }

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    for (auto &i:m_peer_table) {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(i.second);
        addr.sin_port = htons(peer_port);
        
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
            int type = buff[0];
            if (type == 1) {
                struct HelloPacket hello;
                if (ParseHello(buff, &hello) >= 0) {
                    const char* peer_addr = inet_ntoa(client_addr.sin_addr);
                    fprintf(stdout, "Get Hello from:id[%hu], ip[%s]\n", hello.router_id, peer_addr);
                    if (m_peer_table.count(hello.router_id) == 0) {
                        m_peer_table[hello.router_id] = peer_addr;
                        SendHello(peer_addr, 12345);
                        fprintf(stdout, "Send Hello to:id[%hu], ip[%s]\n", hello.router_id, peer_addr);
                    }
                }
            } else if (type == 2) {
                uint32_t path_num_net;
                memcpy(&path_num_net, buff+4, 4); 
                uint32_t path_num_host = ntohl(path_num_net);
                int adver_len = sizeof(struct AdvertisePacket)+sizeof(struct PathInPacket)*path_num_host;
                struct AdvertisePacket* advertise = (struct AdvertisePacket*)malloc(adver_len);
                advertise->path_num = path_num_host;
                ParseAdvertise(buff, advertise);
                fprintf(stdout, "Get Adver from id[%hu], number of path: [%hu]\n", advertise->router_id, advertise->path_num);
                free(advertise);
            } else {
                fprintf(stderr, "Unknown packet type:[%d]\n",type);
                return -1;
            }
        }
    }

    close(socket);
    return 0;
}

int Router::ParseHello(const char* data, struct HelloPacket* hello) {
    uint16_t id_net;
    uint32_t as_net;
    memcpy(&id_net, data+2, 2);
    memcpy(&as_net, data+4, 4); 
    hello->router_id = ntohs(id_net);
    hello->AS_number = ntohl(as_net);
    if (hello->AS_number != m_AS_number) {
        fprintf(stdout, "Get Hello from diff AS[%hu]\n", hello->AS_number);
        return -1;
    }
    return 0;
}

int Router::ParseAdvertise(const char* data, struct AdvertisePacket* advertise) {
    uint16_t id_net;
    memcpy(&id_net, data+2, 2);
    advertise->router_id = ntohs(id_net);
    for (auto i = 0;i < advertise->path_num;++i) {
        PathInPacket pip;
        uint16_t sid_n, label_n;
        memcpy(&sid_n, data+8+i*16, 2);
        memcpy(&label_n, data+10+i*16, 2);
        pip.sid = ntohs(sid_n);
        pip.in_label = ntohs(label_n);

        Attribute attr;
        uint32_t delay_n, bw_n, com_n;
        memcpy(&delay_n, data+12+i*16, 4);
        memcpy(&bw_n, data+16+i*16, 4);
        memcpy(&com_n, data+20+i*16, 4);
        attr.delay = ntohl(delay_n);
        attr.bandwidth = ntohl(bw_n);
        attr.computing_rate = ntohl(com_n);

        pip.attr = attr;

        advertise->paths[i] = pip;
    }
    return 0;
}

void Router::RecvWork() {
    m_recv_thread = std::thread(&Router::RecvPacket, this, m_local_ip);
}

void Router::HelloWork() {
    
}