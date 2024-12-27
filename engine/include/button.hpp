#pragma once

#include "SDL.h"

class Button
{
public:
	Button(SDL_FRect _rect);

	bool is_clicked(int mouse_x, int mouse_y) const;

	SDL_FRect rect;
};