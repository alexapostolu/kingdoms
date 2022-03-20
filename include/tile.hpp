#pragma once

#include <string>

enum class TileState
{
	GRASS,
	PATH,
	OCCUPIED
};

struct Tile
{
	TileState state;
	std::string img;
};