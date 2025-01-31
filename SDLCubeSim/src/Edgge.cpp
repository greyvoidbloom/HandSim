#include "Edge.h"
#include <cmath>

Edge::Edge(float x, float y, float z) : x(x), y(y), z(z) {}

SDL_Point Edge::projectto2D(float fov, int screenWidth, int screenHeight) const {
    float factor = fov / (fov + z);
    int projX = static_cast<int>(x * factor) + screenWidth / 2;
    int projY = static_cast<int>(y * factor) + screenHeight / 2;

    return SDL_Point{projX, projY};
}
