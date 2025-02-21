cmake_minimum_required(VERSION 3.16)

project(SDL2Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

# Include SDL2 headers manually
include_directories(${INCLUDE_DIR})

# Add source files
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

# Create executable
add_executable(sdlCube ${SRC_FILES})

# Platform-specific handling for macOS and Linux

if(APPLE)
    # macOS-specific configuration: Link required macOS frameworks
    target_link_libraries(sdlCube PRIVATE ${LIB_DIR}/libSDL2.dylib)

    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

    target_link_libraries(sdlCube PRIVATE ${COCOA_FRAMEWORK} ${IOKIT_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK})

    # Explicitly set macOS SDK if required
    execute_process(COMMAND xcrun --sdk macosx --show-sdk-path OUTPUT_VARIABLE CMAKE_OSX_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT})

elseif(UNIX)
    # Linux-specific configuration

    # Manually specify the SDL2 include and library directories
    include_directories(${INCLUDE_DIR})

    # Check if we are on Linux and use the .so file
    if(EXISTS "${LIB_DIR}/libSDL2.so")
        target_link_libraries(sdlCube PRIVATE ${LIB_DIR}/libSDL2.so)
    else()
        message(FATAL_ERROR "SDL2 library (libSDL2.so) not found in ${LIB_DIR}. Please ensure it is in the 'lib' folder.")
    endif()

endif()