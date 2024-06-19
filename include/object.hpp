#pragma once

#include "SDL.h"

namespace king {

class Object
{
protected:
	void pan_point(SDL_FPoint& point, float dx, float dy);
};

} // namespace king