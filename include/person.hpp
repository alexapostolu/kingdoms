#pragma once

#include "tile.hpp"

#include <SDL.h>

#include <random>
#include <vector>

struct Person
{
    void generate_path(std::vector<std::vector<Tile>> const& tiles);

    float x, y;
    std::vector<SDL_FPoint> path;

    static std::random_device dev;
    static std::mt19937 eng;
    static std::uniform_int_distribution<std::mt19937::result_type> distrx, distry;
};