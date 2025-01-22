cmake_minimum_required(VERSION 3.16)
project(Cuber LANGUAGES CXX C)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the C standard for glad.c
set(CMAKE_C_STANDARD 99)

# Add compiler options
add_compile_options(-Wall -fdiagnostics-color=always -Wno-deprecated)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/dependencies/include)

# Add the GLFW library
set(GLFW_LIB ${CMAKE_SOURCE_DIR}/dependencies/library/libglfw.3.4.dylib)

# Add glad.c and main.cpp
add_executable(${PROJECT_NAME} main.cpp glad.c)

# Link the GLFW library and macOS frameworks
target_link_libraries(${PROJECT_NAME}
    ${GLFW_LIB}
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework CoreFoundation"
)
