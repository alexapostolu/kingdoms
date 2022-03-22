#pragma once

#include "SDL.h"
#include "person.hpp"
#include "tile.hpp"

#include <random>
#include <string>
#include <vector>

class Base
{
public:
    static Base& get();

public:
    Base(Base const&) = delete;
    void operator=(Base const&) = delete;

public:
    void display_resources();
    void display_scene();
    void display_shop();

    void handle_mouse_on_shop(int x, int y);

public:
	int gold, wheat, wood, stone, gems;
	int level, exp, troph;
    bool edit_mode;

    int const TILES_X, TILES_Y;

    enum class ShopState
    {
        HIDDEN,
        VISIBLE,
        APPEARING,
        DISAPPEARING
    } shop_state;

private:
    Base();

    std::vector<std::vector<Tile>> tiles;
    std::vector<Person> farmers;
    std::vector<std::pair<std::string, std::vector<int> > > buildings;
    // building being placed
    int place;
};