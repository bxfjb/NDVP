// "ndvp" stands for "New Distance Vector Protocol"

#include <iostream>
#include "ndvp/ndvp.h"
#include "ndvp/util.h"


int main() {
    auto r1 = std::make_shared<Router>(1,1,"10.0.2.15");
    auto r2 = std::make_shared<Router>(2,1,"127.0.0.1");
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

    r1->SendHello(r2->m_local_ip, 12345);
    r1->m_peer_table[2] = "127.0.0.1";
    r1->m_adj_out.emplace_back(p);
    
    r1->SendAdvertise();
}
