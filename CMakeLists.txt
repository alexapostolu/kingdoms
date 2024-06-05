cmake_minimum_required(VERSION 3.10)
project(kingdoms)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/build")

# Add the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# SDL2
set(SDL2_DIR ${PROJECT_SOURCE_DIR}/external/SDL2)
set(CMAKE_PREFIX_PATH ${SDL2_DIR})
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# src directory
add_subdirectory(src)
