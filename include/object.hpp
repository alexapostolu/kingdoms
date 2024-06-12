#pragma once

#include "SDL.h"

namespace king {

// The purpose of this class is to offset and scale the object when the screen
// is panned or zoomed in/out
struct Object
{
	float x, y;
	float old_scale;

	Object(float _x, float _y);
	void pan(float dx, float dy);
	void zoom(SDL_MouseWheelEvent const& wheel);
	virtual void render(SDL_Renderer* renderer) const;
};

struct Farmhouse : public Object
{
	SDL_Texture* texture;
	int width, height;

	void init(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer) const override;
};

} // namespace king