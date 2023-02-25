//
// Created by Lenovo on 2023/2/24.
//

#include <sstream>
#include <fstream>
#include <iomanip>

#include "nap.h"
#include "../ndvp/util.h"

#define HEX( x )  \
   std::setw(2) << std::setfill('0') << std::hex << (int)( x )

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
                fprintf(stdout, "Receive NAP Request packet\n");
                uint16_t cri_n;
                memcpy(&cri_n, buff+4, 2);
                uint16_t cri = ntohs(cri_n);
                int r_len = info_len[cri] + sizeof(RequestPacket);
                struct RequestPacket* request = (struct RequestPacket*)malloc(r_len);
                if (ParseRequest(buff, request) >= 0) {
                    fprintf(stdout, "PArse NAP Request packet success, stream_number[%hu]\n", request->stream_num);
                    m_router->CalculateBestPath(request);
                    client_addr.sin_port = htons(NAP_PORT);
                    SendResponse((struct sockaddr*)&client_addr, request->stream_num);
                }
                free(request);
            } else if (type == 3) {
                fprintf(stdout, "Receive NAP Payload packet\n");
                uint16_t content_len_n;
                memcpy(&content_len_n, buff+2, 2);
                uint16_t content_len = ntohs(content_len_n);
                int p_len = 8 + content_len*16;
                struct PayloadPacket *payload = (struct PayloadPacket*)malloc(p_len);
                if (ParsePayload(buff, payload) >= 0) {
                    fprintf(stdout, "Parse NAP Payload packet success, length[%d]\n", p_len);
                    uint16_t out_label = 0;
                    std::pair<uint16_t,std::string> next;
                    if (payload->label == 0) {
                        next = m_router->GetLabelByNumber(payload->stream_number);
                    } else {
                        next = m_router->GetLabelByLabel(payload->label);
                    }
                    if (next.first == 0) {
                        fprintf(stdout, "Enter Egress node\n");
                    } else if (!next.second.empty()) {
                        
                        Forward(buff, next.first, next.second, p_len);
                        fprintf(stdout, "Forward payload to [%s]\n", next.second.c_str());
                    }
                }
                free(payload);
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

    uint16_t content_len = ntohs(content_length_net);
    memcpy(payload->data, data+8, content_len*16);
    return 0;
}

int Server::SendResponse(sockaddr *client_addr, uint16_t stream_number)
{
    int socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    ResponsePacket response;
    response.type = 2;
    response.checksum = 0;
    response.stream_num = htons(stream_number);

    int ret = sendto(socket, (char*)&response, sizeof(ResponsePacket), 0, client_addr, sizeof(struct sockaddr));
    if (ret < 1) {
        perror("SendResponse sendto");
        return -1;
    }

    close(socket);

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

    int opt = 1;
    setsockopt(socket,SOL_SOCKET, SO_BROADCAST,&opt,sizeof(opt));

    uint16_t out_label_net = htons(out_label);
    memcpy(data+4, &out_label_net, sizeof(uint16_t));

    ret = sendto(socket, data, size, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("Forward sendto");
        return -1;
    }

    close(socket);

    return 0;
}

int Client::SendRequest() {
    int ret, socket;
    struct sockaddr_in addr;

    struct RequestPacket* request = (struct RequestPacket*)malloc(m_request_len);
    request->type = 1;
    request->checksum = 0;
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

    int opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ret = sendto(socket, (char *)request, m_request_len, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("SendRequest sendto");
        return -1;
    }
    fprintf(stdout, "Send request len[%d]: type[%u], checksum[%u], sid[%hu], criteria[%hu], stream_number[%hu]\n", 
                    m_request_len, request->type, request->checksum, request->sid, request->criteria, request->stream_num);

    close(socket);

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    socklen_t sock_len = sizeof(struct sockaddr);
    ResponsePacket response;
    struct sockaddr_in bind_addr;

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr =  htonl(INADDR_ANY);
    bind_addr.sin_port = htons(NAP_PORT);

    if (bind(socket, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
        perror("SendRequest bind");
        return -1;
    }
    ret = recvfrom(socket, (char*)&response, sizeof(ResponsePacket), 0, (struct sockaddr *) &addr, &sock_len);
    if (ret > 0) {
        fprintf(stdout, "Receive Response\n");
        if (m_request && m_request->stream_num == ntohs(response.stream_num)) {
            fprintf(stdout, "Parse Response success, send payload\n");
            SendPayload();
        }
    }

    close(socket);

    return 0;
}

int Client::SendPayload()
{
    fprintf(stdout, "Send Payload...\n");
    int ret, socket;
    struct sockaddr_in addr;

    int p_len = m_payload.size() + sizeof(PayloadPacket);
    struct PayloadPacket* payload = (struct PayloadPacket*)malloc(p_len);
    payload->type = 3;
    payload->checksum = 0;
    payload->content_length = htons((uint16_t)m_payload.size()/16);
    payload->label = 0;
    payload->stream_number = htons(m_request->stream_num);
    memcpy(payload->data, m_payload.c_str(), m_payload.size());

    addr.sin_family = AF_INET;
    addr.sin_port = htons(NAP_PORT);
    addr.sin_addr.s_addr = inet_addr(m_server_addr.c_str());

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    int sock_opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    ret = sendto(socket, (char *)payload, p_len, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("SendPayload sendto");
        return -1;
    }

    close(socket);
    fprintf(stdout, "Send payload length[%d], payload:[%s]\n", p_len, payload->data);

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
        m_request_len = info_len[criteria] + sizeof(RequestPacket);
        m_request = (struct RequestPacket*)malloc(m_request_len);
        m_request->sid = sid;
        m_request->criteria = criteria;
        m_request->stream_num = stream_number;
        if (criteria >= 3) {datastr >> m_request->info[0];}
        if (criteria >= 5) {datastr >> m_request->info[1];}
        if (criteria >= 6) {datastr >> m_request->info[2];}
        //fprintf(stdout, "Save com data sid:%hu, com:%u\n", sid, com);
        datastr.clear();
    }
    fprintf(stdout, "Read in request len[%d]: sid[%hu], criteria[%hu], stream_number[%hu]\n", 
                                    m_request_len, m_request->sid, m_request->criteria, m_request->stream_num);
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

    fprintf(stdout, "Read in payload: %s\n", m_payload.c_str());
}
