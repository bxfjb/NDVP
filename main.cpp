// "ndvp" stands for "New Distance Vector Protocol"

#include <iostream>
#include <string>
#include "ndvp/ndvp.h"
#include "ndvp/util.h"

#define AS_NUMBER 33
#define MASK "255.255.0.0"

// NetworkSystem local_id local_ip [peer_ip]
int main(int argc,char *argv[]) {
    int router_id = atoi(argv[1]);
    std::cout << "This is router[" << router_id << "]" << std::endl;

    std::vector<std::pair<const char*, const char*>> local_ip;
    for (int i = 2;i < argc;i++) {
        const char* ip = argv[i];
        const char* mask = MASK;
        std::cout << "ip:[" << ip << "], mask:[" << mask << "]" << std::endl;
        local_ip.emplace_back(ip, mask);
    }
    auto r1 = std::make_shared<Router>(router_id,AS_NUMBER,local_ip);
    Attribute attr;
    attr.delay = 3;
    attr.bandwidth = 4;
    attr.computing_rate = 5;
    Path p;
    p.index = 0;
    p.next_hop_id = 2;
    p.in_label = 2;
    p.out_label = 1000;
    p.sid = 1;
    p.attr = attr;

    sleep(10);

    //r1->m_adj_out.emplace_back(&p);

}
