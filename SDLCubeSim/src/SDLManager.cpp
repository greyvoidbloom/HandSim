#include "SDLManager.h"
#include <iostream>
#include <stdexcept>

SDLManager::SDLManager(const char* WindowName) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Initialization failed! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Initialization failed!");
    }

    window = SDL_CreateWindow(WindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Window creation failed!");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed! SDL Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Renderer creation failed!");
    }
}

SDLManager::~SDLManager() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Window* SDLManager::getWindow() {
    return window;
}

SDL_Renderer* SDLManager::getRenderer() {
    return renderer;
}
