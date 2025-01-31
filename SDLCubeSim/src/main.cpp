#include <iostream>
#include <SDL2/SDL.h>
#include "SDLManager.h"
#include "Cube.h"
#include "udpReceiver.h"
#define PORT 5555
#define BUFFER_SIZE 1024
void handleRotationSpeed(int* thumbFinger, int* indexFinger,float *rotationSpeedX, float *rotationSpeedY){
    int dist =static_cast<int>(sqrt(pow((thumbFinger[0] - indexFinger[0]),2) + pow((thumbFinger[1] - indexFinger[1]),2))/10);
    //std::cout << "distance: "<< dist <<std::endl ;
    *rotationSpeedY = dist;
}
void handleRotationDirection(int* thumbFinger, int* indexFinger,float *rotationSpeedX, float *rotationSpeedY){
    int moveDirControl = thumbFinger[0] - indexFinger[0];
    (moveDirControl<0) ? *rotationSpeedY = -*rotationSpeedY : *rotationSpeedY = *rotationSpeedY ;
}
int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }
    struct sockaddr_in serverAddr, clientAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
        close(sockfd);
        return -1;
    }
    // Make the socket non-blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    char buffer[BUFFER_SIZE];
    socklen_t clientAddrLen = sizeof(clientAddr);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;
    try {
        SDLManager sdl("Chopped SDL Cube");
        Cube choppedCube(400,800,600);
        float angleX, angleY = 0.0f;
        float rotationSpeedX = 1.0f;
        float rotationSpeedY = 1.0f;
        int thumbFinger[2] = {0, 0};
        int indexFinger[2] = {0, 0};
        bool running = true;

        SDL_Event event;
        Uint32 prevtime = SDL_GetTicks();

        while (running) {
            Uint32 curTime = SDL_GetTicks();
            float deltaTime = (curTime - prevtime) /1000.f;
            prevtime = curTime;
            bool dataReceived = false;
            if (receiveData(sockfd, buffer, clientAddr,clientAddrLen, thumbFinger, indexFinger)) {
                handleRotationSpeed(thumbFinger,indexFinger,&rotationSpeedX,&rotationSpeedY);
                handleRotationDirection(thumbFinger,indexFinger,&rotationSpeedX,&rotationSpeedY);
                dataReceived = true;
            }
            // Handle SDL events
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;  
                }
            }
            SDL_SetRenderDrawColor(sdl.getRenderer(), 255, 255, 255, 255);
            SDL_RenderClear(sdl.getRenderer());
            //std::cout << rotationSpeedY << std::endl;
            angleY = rotationSpeedY * deltaTime;
            if (dataReceived){
                //choppedCube.rotateX(angleX);
                choppedCube.rotateY(angleY);
            }

           //std::cout <<"angle: "<<angleX <<"increment: "<<angleXincrement<<std::endl;

            choppedCube.renderCube(sdl.getRenderer());
            SDL_RenderPresent(sdl.getRenderer());
            SDL_Delay(1);
            //SDL_Delay(16);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    close(sockfd);

    return 0;
}