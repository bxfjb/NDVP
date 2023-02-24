#include <bits/stdc++.h>

struct Attribute{
    uint32_t delay;
    uint32_t bandwidth;
    uint32_t computing_rate;
    Attribute operator+(Attribute &a1) {
        Attribute a2;
        a2.delay = delay + a1.delay;
        a2.bandwidth = std::min(bandwidth, a1.bandwidth);
        a2.computing_rate = std::max(computing_rate, a1.computing_rate);
        return a2;
    }

    bool operator==(Attribute &a1) {
        return delay == a1.delay && bandwidth == a1.bandwidth && computing_rate == a1.computing_rate;
    }
};

struct PathInPacket {
    uint16_t sid;
    uint16_t in_label;
    Attribute attr;
};

int main() {
    std::stringstream datastr;
    std::string data;
    std::ifstream ifs;
    uint16_t sid;
    uint32_t com;
    
    ifs.open("com.tsv", std::ios::in);
    if(!ifs.is_open())
    {
        std::cerr<<"cannot open the com file\n";
    }
    while (std::getline(ifs, data)) {
        datastr << data;
        datastr >> sid >> com;
        PathInPacket pip;
        pip.sid = sid;
        pip.in_label = 0;
        pip.attr.delay = 0;
        pip.attr.bandwidth = UINT32_MAX;
        pip.attr.computing_rate = com;
        std::cout << pip.sid << " ";
        datastr.clear();
    }
}