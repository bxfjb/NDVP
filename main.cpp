// "ndvp" stands for "New Distance Vector Protocol"

#include <iostream>
#include <string>
#include "ndvp/ndvp.h"
#include "nap/nap.h"
#include "ndvp/util.h"

#define AS_NUMBER 33
#define MASK "255.255.0.0"

// NetworkSystem 0 [0-2] router-id [local_ip] [0-1]
// NetworkSystem 1 server_ip
// NetworkSystem 2 egress_ip
int main(int argc,char *argv[]) {
    if (atoi(argv[1]) == 0)
    {
        int proto_type = atoi(argv[2]);
        int router_id = atoi(argv[3]);
        std::cout << "This is router[" << router_id << "]" << std::endl;

        std::vector<std::pair<const char*, const char*>> local_ip;
        for (int i = 4;i < argc-1;i++) {
            const char* ip = argv[i];
            const char* mask = MASK;
            std::cout << "ip:[" << ip << "], mask:[" << mask << "]" << std::endl;
            local_ip.emplace_back(ip, mask);
        }
        auto r = std::make_shared<Router>(router_id,AS_NUMBER,local_ip,proto_type);
        auto s = std::make_shared<Server>(r);
        if (atoi(argv[argc-1]) == 1) {
            r->Shell();
        }
    }
    else if (atoi(argv[1]) == 1) {
        std::string server_addr = std::string(argv[2]);
        auto c = std::make_shared<Client>(server_addr);
    }
    else if (atoi(argv[1]) == 2) {
        std::string egress_addr = std::string(argv[2]);
        int server_id = atoi(argv[3]);
        auto c = std::make_shared<Router>(egress_addr, server_id);
    }

}
