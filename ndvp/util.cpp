//
// Created by Lenovo on 2023/2/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "util.h"

int NetUtil::makeSocket(int type, int protocol) {
    int s = socket(AF_INET, type, protocol);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return s;
}
