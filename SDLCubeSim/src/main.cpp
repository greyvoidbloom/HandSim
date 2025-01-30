#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <fcntl.h> 

#define PORT 5555
#define BUFFER_SIZE 1024

class Edge{
public:
    float x,y,z;
    Edge(float x, float y, float z): x(x) , y(y) , z(z) {}
    SDL_Point projectto2D(float fov,int screenWidth,int screenHeight) const{
        float factor = fov/(fov+z);
        int projX = static_cast<int>(x*factor) + screenWidth/2;
        int projY = static_cast<int>(y*factor) + screenHeight/2;

        return SDL_Point{projX,projY};
    }
};

class Cube{
private:
    std::vector<Edge> vertices;
    std::vector<std::pair<int,int>> edges;
    float fov;
    int screenWidth,screenHeight;
public:
    
    Cube(float fov,int screenWidth, int screenHeight): fov(fov) , screenWidth(screenWidth) , screenHeight(screenHeight){
        vertices = {
            {-100, -100, -100}, {-100, 100, -100}, {100, 100, -100}, {100, -100, -100},
            {-100, -100, 100}, {-100, 100, 100}, {100, 100, 100}, {100, -100, 100}
        };
        edges = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };
    }
    const std::vector<Edge>& getVertices() const {
        return vertices;
    }
    void rotateX(float angle) {
        for(auto& vertex : vertices){
            float tempY = vertex.y;
            vertex.y = vertex.y * cos(angle) - vertex.z * sin(angle);
            vertex.z = tempY * sin(angle) + vertex.z * cos(angle);
        }
    }
    void rotateY(float angle){
        for (auto& vertex : vertices){
            float tempX = vertex.x;
            vertex.x = vertex.x * cos(angle) + vertex.z * sin(angle);
            vertex.z =  - tempX * sin(angle) + vertex.z * cos(angle);
        }
    }


    void renderCube(SDL_Renderer* renderer) const{
        std::vector<SDL_Point> projectedVertices;
        for (const auto&vertex : vertices){
            SDL_Point projectedVertex = vertex.projectto2D(fov,screenWidth,screenHeight);
            projectedVertices.push_back(projectedVertex);
        }
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        for (const auto& edge : edges){
            SDL_RenderDrawLine(renderer,projectedVertices[edge.first].x,projectedVertices[edge.first].y,
                                projectedVertices[edge.second].x,projectedVertices[edge.second].y);
        }
    }

};

// SDLManager class to manage SDL initialization, window and renderer
class SDLManager {
public:
    const char* WindowName;
    SDLManager(const char* WindowName) {
        this->WindowName = WindowName;
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL Initialization failed! SDL_Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("SDL Initialization failed!");
        }

        // Create SDL window
        window = SDL_CreateWindow(WindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Window creation failed! SDL_Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Window creation failed!");
        }

        // Create SDL renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer creation failed! SDL Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Renderer creation failed!");
        }
    }

    // Destructor: Cleans up SDL resources
    ~SDLManager() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }

private:
    SDL_Window* window;    // SDL window
    SDL_Renderer* renderer; // SDL renderer
};
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
        float rotationSpeedX = 2.0f;
        float rotationSpeedY = 5.0f;
        int thumbFinger[2] = {0, 0};
        int indexFinger[2] = {0, 0};
        bool running = true;

        SDL_Event event;
        Uint32 prevtime = SDL_GetTicks();

        while (running) {
            Uint32 curTime = SDL_GetTicks();
            float deltaTime = (curTime - prevtime) /1000.f;
            prevtime = curTime;

            if (receiveData(sockfd, buffer, clientAddr,clientAddrLen, thumbFinger, indexFinger)) {
                int moveDirControl = thumbFinger[0] - indexFinger[0];
                //std::cout << "Direction: " << moveDirControl << std::endl;
                angleY = (moveDirControl > 0) ? rotationSpeedY * deltaTime : -rotationSpeedY * deltaTime;
            }
            // Handle SDL events
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;  
                }
            }
            SDL_SetRenderDrawColor(sdl.getRenderer(), 255, 255, 255, 255);
            SDL_RenderClear(sdl.getRenderer());

            //choppedCube.rotateX(angleX);
            choppedCube.rotateY(angleY);

           //std::cout <<"angle: "<<angleX <<"increment: "<<angleXincrement<<std::endl;

            choppedCube.renderCube(sdl.getRenderer());
            SDL_RenderPresent(sdl.getRenderer());

            SDL_Delay(16);

        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    close(sockfd);

    return 0;
}
