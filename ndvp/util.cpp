//
// Created by Lenovo on 2023/2/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "util.h"

int NetUtil::makeSocket(int type, int protocol) {
    int s = socket(AF_INET, type, protocol);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return s;
}

const char *getBroadIp(const char *ip, const char *netmask)
{
    struct in_addr addr;
    struct in_addr mask ;
    struct in_addr broadIp ;
    if(inet_aton(ip, &addr) == 0)
    {
        perror("ip inet_aton error");
        return NULL;
    }
    if(inet_aton(netmask, &mask) == 0)
    {
        perror("mask inet_aton error");
        return NULL;
    }
    broadIp.s_addr = addr.s_addr | (~mask.s_addr) ;
    return inet_ntoa(broadIp) ;
}
