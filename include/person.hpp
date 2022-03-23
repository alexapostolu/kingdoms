#pragma once

#include "tile.hpp"

#include <SDL.h>

#include <random>
#include <deque>

struct Person
{
    void generate_path(std::vector<std::vector<Tile>> const& tiles);

    SDL_Point path_pos;
    SDL_FPoint actual_pos;
    std::deque<SDL_Point> path;

    static std::random_device dev;
    static std::mt19937 eng;
    static std::uniform_int_distribution<std::mt19937::result_type> distrx, distry;
};