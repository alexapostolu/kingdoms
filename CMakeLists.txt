cmake_minimum_required(VERSION 3.0)

project(kingdom LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("SDL" "include")

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/main.cpp")
add_executable(kingdom ${SOURCES})

target_link_libraries(kingdom ${CMAKE_SOURCE_DIR}/SDL2.lib ${CMAKE_SOURCE_DIR}/SDL2main.lib ${CMAKE_SOURCE_DIR}/SDL2_ttf.lib)