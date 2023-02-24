//
// Created by Lenovo on 2023/2/24.
//

#include <sstream>
#include <fstream>

#include "nap.h"
#include "../ndvp/util.h"

void Server::RecvWork() {
    m_recv_thread = std::thread(&Server::RecvPacket, this);
}

int Server::RecvPacket() {
    int ret, socket;
    struct sockaddr_in addr;

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    int opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ntohl(INADDR_ANY);
    addr.sin_port = htons(NAP_PORT);

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
                uint16_t cri_n;
                memcpy(&cri_n, buff+4, 2);
                uint16_t cri = ntohs(cri_n);
                int r_len = info_len[cri] + sizeof(RequestPacket);
                struct RequestPacket* request = (struct RequestPacket*)malloc(r_len);
                if (ParseRequest(buff, request) >= 0) {
                    m_router->CalculateBestPath(request);
                }
                free(request);
            } else if (type == 2) {
                struct PayloadPacket payload;
                if (ParsePayload(buff, &payload) >= 0) {
                    uint16_t out_label = 0;
                    std::pair<uint16_t,std::string> next;
                    if (payload.label == 0) {
                        next = m_router->GetLabelByNumber(payload.stream_number);
                    } else {
                        next = m_router->GetLabelByLabel(payload.label);
                    }
                    Forward(buff, next.first, next.second, payload.content_length*16+8);
                }
            } else {
                fprintf(stderr, "Unknown NAP packet type:[%d]\n",type);
                return -1;
            }
        }
    }

    close(socket);
    return 0;
}

int Server::ParseRequest(const char *data, struct RequestPacket *request) {
    uint16_t sid_net, criteria_net, stream_num_net;
    memcpy(&sid_net, data+2, 2);
    memcpy(&criteria_net, data+4, 2);
    memcpy(&stream_num_net, data+6, 2);

    request->sid = ntohs(sid_net);
    request->criteria = ntohs(criteria_net);
    request->stream_num  = ntohs(stream_num_net);
    if (request->criteria == 3) {
        uint32_t k_net;
        memcpy(&k_net, data+8, 4);
        request->info[0] = ntohl(k_net);
    } else if (request->criteria == 4) {
        uint32_t w_net;
        memcpy(&w_net, data+8, 4);
        request->info[0] = ntohl(w_net);
    } else if (request->criteria == 5) {
        uint32_t k_net, k1_net;
        memcpy(&k_net, data+8, 4);
        memcpy(&k1_net, data+12, 4);
        request->info[0] = ntohl(k_net);
        request->info[1] = ntohl(k1_net);
    } else if (request->criteria == 6) {
        uint32_t k_net, k1_net, w_net;
        memcpy(&k_net, data+8, 4);
        memcpy(&k1_net, data+12, 4);
        memcpy(&w_net, data+16, 4);
        request->info[0] = ntohl(k_net);
        request->info[1] = ntohl(k1_net);
        request->info[2] = ntohl(w_net);
    }
    return 0;
}

int Server::ParsePayload(const char *data, struct PayloadPacket *payload) {
    uint16_t content_length_net, label_net, stream_num_net;
    memcpy(&label_net, data+2, 2);
    memcpy(&label_net, data+4, 2);
    memcpy(&stream_num_net, data+6, 2);
    payload->content_length = ntohs(content_length_net);
    payload->label = ntohs(label_net);
    payload->stream_number = ntohs(stream_num_net);
    return 0;
}

int Server::Forward(char *data, uint16_t out_label, std::string next_ip, int size)
{
    int ret, socket;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NAP_PORT);
    addr.sin_addr.s_addr = inet_addr(next_ip.c_str());

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    int sock_opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    uint16_t out_label_net = htons(out_label);
    memcpy(data+4, &out_label_net, sizeof(uint16_t));

    ret = sendto(socket, data, size, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("sendto");
        return -1;
    }

    close(socket);

    return 0;
}

int Client::SendRequest() {
    int ret, socket;
    struct sockaddr_in addr;

    int r_len = info_len[m_request->criteria] + sizeof(RequestPacket);
    struct RequestPacket* request = (struct RequestPacket*)malloc(r_len);
    request->sid = htons(m_request->sid);
    request->criteria = htons(m_request->criteria);
    request->stream_num = htons(m_request->stream_num);
    if (m_request->criteria >= 3) {request->info[0] = htonl(m_request->info[0]);}
    if (m_request->criteria >= 5) {request->info[1] = htonl(m_request->info[1]);}
    if (m_request->criteria == 6) {request->info[2] = htonl(m_request->info[2]);}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(NAP_PORT);
    addr.sin_addr.s_addr = inet_addr(m_server_addr.c_str());

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    int sock_opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    ret = sendto(socket, (char *) &request, sizeof(request), 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("sendto");
        return -1;
    }

    close(socket);

    return 0;
}

int Client::SendPayload()
{
    int ret, socket;
    struct sockaddr_in addr;

    int p_len = m_payload.size() + sizeof(PayloadPacket);
    struct PayloadPacket* payload = (struct PayloadPacket*)malloc(p_len);
    payload->content_length = htons((uint16_t)m_payload.size()/16);
    payload->label = 0;
    payload->stream_number = htons(m_request->stream_num);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(NAP_PORT);
    addr.sin_addr.s_addr = inet_addr(m_server_addr.c_str());

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    int sock_opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    ret = sendto(socket, (char *) &payload, sizeof(payload), 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("sendto");
        return -1;
    }

    close(socket);

    return 0;
}

void Client::read_req_data() {
    std::stringstream datastr;
    std::string data;
    std::ifstream ifs;
    uint16_t sid;
    uint16_t criteria;
    uint16_t stream_number;
    std::string req_file = "./req.tsv";
    ifs.open(req_file, std::ios::in);
    if(!ifs.is_open())
    {
        std::cerr<<"cannot open the req file\n";
    }
    while (std::getline(ifs, data)) {
        datastr << data;
        datastr >> sid >> criteria >> stream_number;
        int r_len = info_len[criteria] + sizeof(RequestPacket);
        m_request = (struct RequestPacket*)malloc(r_len);
        if (criteria >= 3) {datastr >> m_request->info[0];}
        if (criteria >= 5) {datastr >> m_request->info[1];}
        if (criteria >= 6) {datastr >> m_request->info[2];}
        //fprintf(stdout, "Save com data sid:%hu, com:%u\n", sid, com);
        datastr.clear();
    }
}

void Client::read_payload_data()
{
    std::ifstream ifs;
    ifs.open("./payload.data", std::ios::in);

    std::ostringstream tmp;
    tmp << ifs.rdbuf();
    m_payload = tmp.str();

    if (m_payload.size() % 16 == 0)
        return;
    int zero_num = 16 - m_payload.size() % 16;
    for (int i = 0;i < zero_num;++i)
        m_payload += "0";
}
