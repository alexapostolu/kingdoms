#pragma once

#include "object.hpp"

#include "SDL.h"

namespace king {

class Grid : private Object
{
public:
	Grid(int _width, int _height);
	~Grid();

public:
	void mouse_wheel(SDL_MouseWheelEvent const& wheel);
	void mouse_drag(float dx, float dy);
	void render(SDL_Renderer* renderer);

public:
	SDL_FPoint** data;
	int width, height;

	float old_scale;
};

}