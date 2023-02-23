//
// Created by Lenovo on 2023/2/21.
//

#include "ndvp.h"
#include "util.h"
#include <string.h>
#include <iostream>


int Router::SendHello() {
    int ret, socket;
    struct sockaddr_in addr;

    struct HelloPacket hello;
    hello.router_id = htons(m_router_id);
    hello.AS_number = htonl(m_AS_number);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(NDVP_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);
    int sock_opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    ret = sendto(socket, (char *) &hello, sizeof(hello), 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 1) {
        perror("sendto");
        return -1;
    }

    close(socket);

    return 0;
}

int Router::ResponseHello(const char* peer_addr) {
    int ret, socket;
    struct sockaddr_in addr;

    struct HelloPacket hello;
    hello.router_id = htons(m_router_id);
    hello.AS_number = htonl(m_AS_number);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(NDVP_PORT);
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
    std::lock_guard<std::mutex> lock(m_adj_out_mutex);
    int ret, socket;
    struct sockaddr_in addr;
    int s = m_adj_out.size();

    int adver_len = sizeof(struct AdvertisePacket)+sizeof(struct PathInPacket)*s;
    auto adver = (struct AdvertisePacket *)malloc(adver_len);
    memset(adver, 0, adver_len);
    adver->type = 2;
    adver->checksum = 0;
    adver->router_id = htons(this->m_router_id);
    adver->path_num = htonl((uint32_t)s);
    
    for (int i = 0;i < s;++i) {
        adver->paths[i].sid = htons(m_adj_out[i]->sid);
        adver->paths[i].in_label = htons(m_adj_out[i]->in_label);
        adver->paths[i].attr.delay = htonl(m_adj_out[i]->attr.delay);
        adver->paths[i].attr.bandwidth = htonl(m_adj_out[i]->attr.bandwidth);
        adver->paths[i].attr.computing_rate = htonl(m_adj_out[i]->attr.computing_rate);
    }
    m_adj_out.erase(m_adj_out.begin(), m_adj_out.begin()+s);

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    for (auto &i:m_peer_table) {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(i.second);
        addr.sin_port = htons(NDVP_PORT);
        
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

int Router::SendAdvertiseInvalid() {
    std::lock_guard<std::mutex> lock(m_adj_outi_mutex);
    int ret, socket;
    struct sockaddr_in addr;
    int s = m_adj_out_invalid.size();

    int adver_len = sizeof(struct AdvertiseInvalidPacket)+sizeof(struct PathInvalidInPacket)*s;
    auto adver = (struct AdvertiseInvalidPacket *)malloc(adver_len);
    memset(adver, 0, adver_len);
    adver->type = 3;
    adver->checksum = 0;
    adver->router_id = htons(this->m_router_id);
    adver->path_num = htonl((uint32_t)s);
    
    for (int i = 0;i < s;++i) {
        adver->paths[i].sid = htons(m_adj_out_invalid[i].sid);
        adver->paths[i].in_label = htons(m_adj_out_invalid[i].in_label);
    }
    m_adj_out_invalid.erase(m_adj_out_invalid.begin(), m_adj_out_invalid.begin()+s);

    socket = NetUtil::makeSocket(SOCK_DGRAM, 0);

    for (auto &i:m_peer_table) {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(i.second);
        addr.sin_port = htons(NDVP_PORT);
        
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

    int opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr =  htonl(INADDR_ANY); //inet_addr(local_ip);
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
                        ResponseHello(peer_addr);
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
            } else if (type == 3) {
                uint32_t path_num_net;
                memcpy(&path_num_net, buff+4, 4); 
                uint32_t path_num_host = ntohl(path_num_net);
                int adver_len = sizeof(struct AdvertiseInvalidPacket)+sizeof(struct PathInvalidInPacket)*path_num_host;
                struct AdvertiseInvalidPacket* advertise = (struct AdvertiseInvalidPacket*)malloc(adver_len);
                advertise->path_num = path_num_host;
                ParseAdvertiseInvalid(buff, advertise);
                fprintf(stdout, "Get AdverInvalid from id[%hu], number of path: [%hu]\n", advertise->router_id, advertise->path_num);
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
    std::lock_guard<std::mutex> lock(m_adj_in_mutex);
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

        m_adj_in.emplace_back(pip,advertise->router_id);
    }
    return 0;
}

int Router::ParseAdvertiseInvalid(const char* data, struct AdvertiseInvalidPacket* advertise) {
    std::lock_guard<std::mutex> lock(m_adj_ini_mutex);
    uint16_t id_net;
    memcpy(&id_net, data+2, 2);
    advertise->router_id = ntohs(id_net);
    for (auto i = 0;i < advertise->path_num;++i) {
        PathInvalidInPacket piip;
        uint16_t sid_n, label_n;
        memcpy(&sid_n, data+8+i*4, 2);
        memcpy(&label_n, data+10+i*4, 2);
        piip.sid = ntohs(sid_n);
        piip.in_label = ntohs(label_n);

        advertise->paths[i] = piip;

        m_adj_in_invalid.emplace_back(piip,advertise->router_id);
    }
    return 0;
}

void Router::DealAdjIn() {
    std::lock_guard<std::mutex> lock(m_adj_in_mutex);
    int s = m_adj_in.size();
    for (int i = 0;i < s;++i) {
        auto pathInPacket = m_adj_in[i];
        auto path = format_trans(pathInPacket.first, pathInPacket.second);
        if (m_rib.count(path.sid) == 0) {
            m_rib.emplace(path.sid, std::vector<Path>());
            m_rib[path.sid].push_back(path);
        } else {
            auto &path_set = m_rib[path.sid];
            bool accept = true;
            for (auto i = path_set.begin();i!= path_set.end();++i) {
                auto old_path = *i;
                if (m_proto_type == 0) {
                    if (better_ndvp(old_path.attr, path.attr)) {
                        accept = false;
                        break;
                    } else if (better_ndvp(path.attr, old_path.attr)) {
                        delete_path(old_path);
                        break;
                    }
                } else if (m_proto_type == 1) {
                    if (better_pdp(old_path.attr, path.attr)) {
                        accept = false;
                        break;
                    } else if (better_pdp(path.attr, old_path.attr)) {
                        delete_path(old_path);
                        break;
                    }
                } else if (m_proto_type == 2) {
                    if (better_eigrp(old_path.attr, path.attr)) {
                        accept = false;
                        break;
                    } else {
                        delete_path(old_path);
                        break;
                    }
                } else {
                    fprintf(stderr, "DealAdjIn() Unknown m_proto_type = %d\n", m_proto_type);
                    exit(-1);
                }
            }
            if (accept) {
                add_path(path_set, path);
            }
        }
    }
    m_adj_in.erase(m_adj_in.begin(), m_adj_in.begin()+s);
}

void Router::DealAdjInInvalid(){
    std::lock_guard<std::mutex> lock(m_adj_ini_mutex);
    int s = m_adj_in_invalid.size();
    for (int i = 0;i < s;++i) {
        auto pathInValidInPacket = m_adj_in_invalid[i];
        auto tar_path = pathInValidInPacket.first;
        if (m_rib.count(pathInValidInPacket.second) != 0) {
            auto path_set = m_rib[pathInValidInPacket.second];
            for (auto &old_path:path_set) {
                if (old_path.out_label == tar_path.in_label) {
                    delete_path(old_path);
                    break;
                }
            }
        }
    }
    m_adj_in_invalid.erase(m_adj_in_invalid.begin(), m_adj_in_invalid.begin()+s);
}

void Router::RecvWork() {
    for (auto &addr:m_local_ip)
        m_recv_threads.emplace_back(&Router::RecvPacket, this, addr.first);
    m_adj_in_thread = std::thread(&Router::CheckAdjIn, this);
    m_adj_ini_thread = std::thread(&Router::CheckAdjInInvalid, this);
    m_adj_out_thread = std::thread(&Router::CheckAdjOut, this);
    m_adj_outi_thread = std::thread(&Router::CheckAdjOutInvalid, this);

    sleep(FIRST_HELLO);
    SendHello();
}

void Router::CheckAdjIn() {
    while (1) {
        sleep(0);
        if (!m_adj_in.empty()) {
            DealAdjIn();
        }
    }
}

void Router::CheckAdjOut() {
    while (1) {
        sleep(0);
        if (!m_adj_out.empty()) {
            SendAdvertise();
        }
    }
}

void Router::CheckAdjInInvalid() {
    while (1) {
        sleep(0);
        if (!m_adj_in_invalid.empty()) {
            DealAdjInInvalid();
        }
    }
}

void Router::CheckAdjOutInvalid() {
    while (1) {
        sleep(0);
        if (!m_adj_out_invalid.empty()) {
            SendAdvertiseInvalid();
        }
    }
}

void Router::ShowInfo() {
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Peer Table" << std::endl;
    for (auto &i:m_peer_table) {
        std::cout << "Peer id:[" << i.first << "], IP:[" << i.second << "]" << std::endl;
    }
}

Path Router::format_trans(PathInPacket &pip, uint16_t rid) {
    Path ret;
    ret.index = m_path_index;
    ret.next_hop_id = rid;
    ret.in_label = m_next_label;
    ret.out_label = pip.in_label;
    ret.attr = pip.attr + m_edge_table[rid];
    return ret;
}

bool Router::better_ndvp(Attribute &a1, Attribute &a2)
{
    if (a1.delay <= a2.delay && a1.bandwidth >= a2.bandwidth && a1.computing_rate >= a2.computing_rate && !(a1 == a2)) {
        return true;
    }
    return false;
}

bool Router::better_pdp(Attribute &a1, Attribute &a2)
{
    if (a1.delay <= a2.delay && a1.bandwidth >= a2.bandwidth && 
                                    !(a1.delay == a2.delay && a1.bandwidth == a2.bandwidth)) {
        return true;
    }
    return false;
}

bool Router::better_eigrp(Attribute &a1, Attribute &a2)
{
    double m1 = 256*((double)10000000/a1.bandwidth + a1.delay);
    double m2 = 256*((double)10000000/a2.bandwidth + a2.delay);
    return m1 <= m2;
}

void Router::add_path(std::vector<Path> &path_set, Path &path)
{
    auto i = path_set.insert(path_set.end(), path);
    m_fib.emplace(path.in_label, &i);
    m_next_label++;
    m_path_index++;
}

void Router::delete_path(Path path)
{
    auto i = m_fib[path.in_label];
    m_rib[path.sid].erase(*i);
    m_fib.erase(path.in_label);
    std::lock_guard<std::mutex> lock(m_adj_outi_mutex);
    m_adj_out_invalid.push_back(path);
}
