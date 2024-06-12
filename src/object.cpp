#include "object.hpp"

#include "SDL.h"
#include "SDL_image.h"

king::Object::Object(float _x, float _y)
	: x(_x), y(_y), old_scale(1) { }

void king::Object::pan(float dx, float dy)
{
	x += dx;
	y += dy;
}

void king::Object::zoom(SDL_MouseWheelEvent const& wheel)
{
	int scroll = wheel.y;
	float new_scale = old_scale + scroll * 0.1f;

	// Prevent scrolling too close or far
	if (new_scale < 0.5f || new_scale > 6.f)
		return;

	// Find offset from position to mouse
	// Multiply that by new scale to get new offset
	// Divide by old scale to get relative offset?? - that what gpt4o said and it worked
	// Update the new position to be from the offset of mouse

	int mouse_x = wheel.mouseX;
	int mouse_y = wheel.mouseY;

	x = (x - mouse_x) * new_scale / old_scale + mouse_x;
	y = (y - mouse_y) * new_scale / old_scale + mouse_y;

	old_scale = new_scale;
}

void king::Object::render(SDL_Renderer* renderer) const {}

void king::Farmhouse::init(SDL_Renderer* renderer)
{
	// Load image using SDL_image
	SDL_Surface* surface = IMG_Load("../../assets/farmhouse.png");
	if (!surface)
	{
		SDL_Log("Failed to load image: %s\n", IMG_GetError());
		return;
	}

	width = surface->w;
	height = surface->h;

	// Convert surface to texture
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		SDL_Log("Failed to create texture: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(surface);
}

void king::Farmhouse::render(SDL_Renderer* renderer) const
{
	float w = 200 * old_scale;
	float h = height / (width / 200) * old_scale;

	SDL_FRect dest_rect = {
			x - (w / 2.0), y - (h / 2.0),
			w, h
	};

	SDL_RenderCopyF(renderer, texture, NULL, &dest_rect);
}