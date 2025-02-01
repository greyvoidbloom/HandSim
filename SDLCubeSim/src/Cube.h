#ifndef CUBE_H
#define CUBE_H

#include "Edge.h"
#include <vector>
#include <SDL2/SDL.h>

class Cube {
private:
    std::vector<Edge> vertices;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::pair<int, int>> inneredges;
    float fov;
    int screenWidth, screenHeight;

public:
    Cube(float fov, int screenWidth, int screenHeight);
    void rotateX(float angle);
    void rotateY(float angle);
    void renderCube(SDL_Renderer* renderer) const;
    const std::vector<Edge>& getVertices() const;
};

#endif
