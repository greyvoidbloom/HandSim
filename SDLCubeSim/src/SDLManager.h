#ifndef SDLMANAGER_H
#define SDLMANAGER_H

#include <SDL2/SDL.h>

class SDLManager {
public:
    SDLManager(const char* WindowName);
    ~SDLManager();

    SDL_Window* getWindow();
    SDL_Renderer* getRenderer();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif // SDLMANAGER_H
