#ifndef EDGE_H
#define EDGE_H

#include <SDL2/SDL.h>

class Edge {
public:
    float x, y, z;
    Edge(float x, float y, float z);

    SDL_Point projectto2D(float fov, int screenWidth, int screenHeight) const;
};

#endif
