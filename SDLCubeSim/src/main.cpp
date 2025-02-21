#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <fcntl.h>
#include <unistd.h>
#include "SDLManager.h"
#include "Cube.h"
#include "udpReceiver.h"
#include "RotationControls.h"
#define PORT 5555
#define BUFFER_SIZE 1024

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
    fcntl(sockfd, F_SETFL, O_NONBLOCK); // Set socket to non-blocking
    char buffer[BUFFER_SIZE];
    socklen_t clientAddrLen = sizeof(clientAddr);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    try {
        SDLManager sdl("Chopped SDL Cube");
        RotationHandler spincontrols;
        Cube choppedCube(400, 800, 600);
        bool receivingData = false;
        int thumbFinger[2] = {0, 0};
        int indexFinger[2] = {0, 0};
        bool running = true;
        int speed = 0;
        SDL_Event event;

        while (running) {
            Uint32 frameStart = SDL_GetTicks();
            bool dataReceived = false;

            // Receive UDP data
            if (receiveData(sockfd, buffer, clientAddr, clientAddrLen, thumbFinger, indexFinger)) {
                //speen
                receivingData = true;
                spincontrols.enableGestureRotation();
                spincontrols.handleXvelocity(thumbFinger,indexFinger);
                spincontrols.handleYvelocity(thumbFinger,indexFinger);
                spincontrols.handleXDirection(thumbFinger,indexFinger);
                spincontrols.handleYDirection(thumbFinger,indexFinger);
            }
            else{
                receivingData = false;
            }
            // Handle SDL events
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_q){running = false;}
                    if (event.key.keysym.sym == SDLK_d && !spincontrols.isKeyboardRotationAllowed()) {
                        spincontrols.enableKeyboardRotation();
                        speed = 7;
                    }
                    if (event.key.keysym.sym == SDLK_a && !spincontrols.isKeyboardRotationAllowed()) {
                        spincontrols.enableKeyboardRotation();
                        speed = -7;
                    }
                    }
            }
            //std::cout << spincontrols.getXangle() <<std::endl;
            if(spincontrols.isKeyboardRotationAllowed()){
                spincontrols.rotateHorizontal(speed);
            }
            if(spincontrols.isGestureRotationAllowed()){
                spincontrols.gestureControlY(&receivingData);
                spincontrols.gestureControlX(&receivingData);
            }
            choppedCube.rotateX(spincontrols.getYangle());
            choppedCube.rotateY(spincontrols.getXangle());
            SDL_SetRenderDrawColor(sdl.getRenderer(), 255, 255, 255, 255);
            SDL_RenderClear(sdl.getRenderer());

            choppedCube.renderCube(sdl.getRenderer());
            SDL_RenderPresent(sdl.getRenderer());
            //frame rate stabilization
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < 16) {
                SDL_Delay(16 - frameTime);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    close(sockfd);
    return 0;
}
