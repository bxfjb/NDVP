//
// Created by Lenovo on 2023/2/21.
//

#ifndef NETWORKSYSTEM_UTIL_H
#define NETWORKSYSTEM_UTIL_H

class NetUtil {
public:
    static int makeSocket(int type, int protocol);
    static const char *getBroadIp(const char *ip, const char *netmask);
};

#endif //NETWORKSYSTEM_UTIL_H
