cmake_minimum_required(VERSION 3.0)

project(kingdom LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

include_directories("assets/SDL" "include")

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR} src/*.cpp)
add_executable(kingdom ${SOURCES})

find_library(SDL2main_LIB "${CMAKE_SOURCE_DIR}/build/SDL2main.lib")
find_library(SDL2_LIB "${CMAKE_SOURCE_DIR}/build/SDL2.lib")
find_library(SDL2ttf_LIB "${CMAKE_SOURCE_DIR}/build/SDL2_ttf.lib")
find_library(SDL2img_LIB "${CMAKE_SOURCE_DIR}/build/SDL2_image.lib")

target_link_libraries(kingdom "${SDL2main_LIB}" "${SDL2_LIB}" "${SDL2mttf_LIB}" "${SDL2img_LIB}")
