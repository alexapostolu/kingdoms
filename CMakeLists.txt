cmake_minimum_required(VERSION 3.10)
project(kingdoms)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/build")

# Add the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# SDL2
set(SDL2_DIR ${PROJECT_SOURCE_DIR}/external/SDL2)
include_directories(${SDL2_DIR}/include)

# SDL2_image
set(SDL2_image_DIR ${PROJECT_SOURCE_DIR}/external/SDL2_image)
include_directories(${SDL2_image_DIR}/include)

# SDL2_ttf
set(SDL2_ttf_DIR ${PROJECT_SOURCE_DIR}/external/SDL2_ttf)
include_directories(${SDL2_ttf_DIR}/include)

# SDL_FontCache
set(SDL_fontcache_DIR ${PROJECT_SOURCE_DIR}/external/SDL_FontCache)
include_directories(${SDL_fontcache_DIR})

# src directory
add_subdirectory(src)