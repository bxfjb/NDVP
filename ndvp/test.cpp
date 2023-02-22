#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

int work(const char* peer_addr) {
    int ret, s;
    struct sockaddr_in addr;

    char buff[] = "test packet";

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(peer_addr);
    addr.sin_port = htons(1234);
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    ret = sendto(s, buff, sizeof(buff), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 1) {
        perror("sendto");
        return -1;
    }

    close(s);

    return 0;
}

int main() {
    work("10.0.2.15");
}