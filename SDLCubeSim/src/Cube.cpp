#include "Cube.h"
#include <cmath>

Cube::Cube(float fov, int screenWidth, int screenHeight)
    : fov(fov), screenWidth(screenWidth), screenHeight(screenHeight) {

    vertices = {
        {-100, -100, -100}, {-100, 100, -100}, {100, 100, -100}, {100, -100, -100},
        {-100, -100, 100}, {-100, 100, 100}, {100, 100, 100}, {100, -100, 100}
    };

    edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    /*inneredges = {
        {0,2},{2,7},{7,5},{5,0}
    };*/
}

void Cube::rotateX(float angle) {
    for (auto& vertex : vertices) {
        float tempY = vertex.y;
        vertex.y = vertex.y * cos(angle) - vertex.z * sin(angle);
        vertex.z = tempY * sin(angle) + vertex.z * cos(angle);
    }
}

void Cube::rotateY(float angle) {
    for (auto& vertex : vertices) {
        float tempX = vertex.x;
        vertex.x = vertex.x * cos(angle) + vertex.z * sin(angle);
        vertex.z = -tempX * sin(angle) + vertex.z * cos(angle);
    }
}

void Cube::renderCube(SDL_Renderer* renderer) const {
    std::vector<SDL_Point> projectedVertices;
    for (const auto& vertex : vertices) {
        SDL_Point projectedVertex = vertex.projectto2D(fov, screenWidth, screenHeight);
        projectedVertices.push_back(projectedVertex);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (const auto& edge : edges) {
        SDL_RenderDrawLine(renderer, projectedVertices[edge.first].x, projectedVertices[edge.first].y,
                                    projectedVertices[edge.second].x, projectedVertices[edge.second].y);
    }

    /*SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto& inneredge : inneredges) {
        SDL_RenderDrawLine(renderer, projectedVertices[inneredge.first].x, projectedVertices[inneredge.first].y,
                                    projectedVertices[inneredge.second].x, projectedVertices[inneredge.second].y);
    }*/
}

const std::vector<Edge>& Cube::getVertices() const {
    return vertices;
}
