#pragma once

#include "sdl2.hpp"

#include <string>

struct Building
{
	std::string img;
	sdl2::Dimension dim;
	int cost_gold, cost_wood, cost_stone;
};