// "ndvp" stands for "New Distance Vector Protocol"

#include <iostream>
#include <string>
#include "ndvp/ndvp.h"
#include "ndvp/util.h"

#define AS_NUMBER 33

// NetworkSystem local_id local_ip [peer_ip]
int main(int argc,char *argv[]) {
    int router_id = atoi(argv[1]);
    std::cout << "This is router[" << router_id << "]" << std::endl;

    std::vector<std::pair<const char*, const char*>> local_ip;
    for (int i = 2;i < argc;i+=2) {
        const char* ip = argv[i];
        const char* mask = argv[i+1];
        std::cout << "ip:[" << ip << "], mask:[" << mask << "]" << std::endl;
        local_ip.emplace_back(ip, mask);
    }
    auto r1 = std::make_shared<Router>(router_id,AS_NUMBER,local_ip);
    Attribute attr;
    attr.delay = 6;
    attr.bandwidth = 7;
    attr.computing_rate = 8;
    Path p;
    p.index = 0;
    p.next_hop_id = 2;
    p.in_label = 3;
    p.out_label = 4;
    p.sid = 5;
    p.attr = attr;

    sleep(5);

    r1->SendHello();
    r1->ShowInfo();
    r1->m_adj_out.emplace_back(&p);
    r1->m_adj_out.emplace_back(&p);
    
    r1->SendAdvertise(12345);
}
