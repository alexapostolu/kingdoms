#pragma once

#include "SDL.h"

/*
 * The standard action of an Object is to handle house presses (eg. resource collection),
 * mouse drag (eg. drag and relocate) and mouse release (clear grid background).
 * It should also provide a pointer that points to the active object in the
 * collection. This should be set by the caller to know which object to drag
 * when mouse is pressed and dragged.
 * Note that mouse drag should only be applied to the active object, not the
 * entire collection.
 * 
 * mx, my - mouse x and y position
 * dx, dy - delta/change in x and y position
 */
class Object
{
protected:
	void pan_point(SDL_FPoint& point, float dx, float dy);
};