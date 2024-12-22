#include "object.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <vector>

void Object::pan_point(SDL_FPoint& point, float dx, float dy)
{
	point.x += dx;
	point.y += dy;
}