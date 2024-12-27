#include "button.hpp"

#include "SDL.h"

Button::Button(SDL_FRect _rect)
	: rect(_rect) {}

bool Button::is_clicked(int mouse_x, int mouse_y) const
{
	return mouse_x > rect.x && mouse_x < rect.x + rect.w &&
		   mouse_y > rect.y && mouse_y < rect.y + rect.h;
}