#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>

int work(){
    int sockfd_one;
    int err_log;
    sockfd_one = socket(AF_INET, SOCK_DGRAM, 0); //创建UDP套接字one
    if(sockfd_one < 0)
    {
        perror("sockfd_one");
        exit(-1);
    }
    int opt = 1;
    // sockfd为需要端口复用的套接字
    setsockopt(sockfd_one, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    // 设置本地网络信息
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(8000);		// 端口为8000
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定，端口为8000
    err_log = bind(sockfd_one, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if(err_log != 0)
    {
        perror("bind sockfd_one");
        close(sockfd_one);
        exit(-1);
    }

    char buff[1024];
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    std::cout << "listening..." << std::endl;
    recvfrom(sockfd_one, buff, 1024, 0, (sockaddr*)&client_addr, &len);

    close(sockfd_one);

    return 0;
}

int main(int argc, char *argv[])
{
    std::thread t1(work);
    std::thread t2(work);
    t1.join();
    t2.join();
}