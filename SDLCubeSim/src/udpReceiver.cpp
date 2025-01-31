#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "udpReceiver.h"
#define PORT 5555
#define BUFFER_SIZE 1024

bool receiveData(int sockfd, char* buffer, sockaddr_in& clientAddr, socklen_t& clientAddrLen, int* thumbFinger, int* indexFinger) {
    ssize_t recvLen = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (recvLen < 0) {
        return false;
    }

    buffer[recvLen] = '\0';
    if (sscanf(buffer, "(%d, %d),(%d, %d)", &thumbFinger[0], &thumbFinger[1], &indexFinger[0], &indexFinger[1]) == 4) {
        return true;
    } else {
        std::cerr << "Failed to parse data!" << std::endl;
        return false;
    }
}
