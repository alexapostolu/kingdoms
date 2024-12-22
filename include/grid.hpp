#pragma once

#include "object.hpp"

#include "SDL.h"

class Grid : private Object
{
public:
	Grid(int _width, int _height);
	~Grid();

public:
	void mouse_wheel(int mouse_x, int mouse_y, float scale_ratio);
	void mouse_drag(float dx, float dy);
	void render(SDL_Renderer* renderer, float scale);

public:
	int width, height;
	SDL_FPoint** data;
};