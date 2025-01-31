
#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
bool receiveData(int sockfd, char* buffer, sockaddr_in& clientAddr, socklen_t& clientAddrLen, int* thumbFinger, int* indexFinger);

#endif 
