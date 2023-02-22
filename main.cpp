// "ndvp" stands for "New Distance Vector Protocol"

#include <iostream>
#include <string>
#include "ndvp/ndvp.h"
#include "ndvp/util.h"

#define AS_NUMBER 33

// NetworkSystem local_id local_ip [peer_ip]
int main(int argc,char *argv[]) {
    int router_id = atoi(argv[1]);
    const char* local_ip = argv[2];
    auto r1 = std::make_shared<Router>(router_id,AS_NUMBER,local_ip);
    std::cout << "id:" << router_id <<  " ip:" << local_ip << std::endl;
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

    for (int i = 3;i < argc;++i) {
        const char* peer_ip = argv[i];
        std::cout << "peer:" << peer_ip << std::endl;
        r1->SendHello(peer_ip, 12345);
    }

    sleep(5);

    r1->ShowInfo();
    r1->m_adj_out.emplace_back(&p);
    r1->m_adj_out.emplace_back(&p);
    
    r1->SendAdvertise(12345);
}
