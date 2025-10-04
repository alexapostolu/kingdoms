#include "image.hpp"

#include "SDL.h"
#include "SDL_Image.h"


night::image::image(
	std::string const& image_path,
	SDL_Renderer* _renderer,
	SDL_Rect const& _texture_rect
)
	: renderer(_renderer)
	, texture(nullptr)
	, texture_rect(_texture_rect)
{
	SDL_Surface* surface = IMG_Load(image_path.c_str());
	if (!surface)
	{
		SDL_Log("%s: failed to load image \"%s\"\n", __func__, IMG_GetError());
		return;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
		SDL_Log("%s: failed to create texture \"%s\"\n", __func__, SDL_GetError());

	if (!texture_rect.x)
		SDL_Log("%s: x position unspecified, setting x to 0");
	if (!texture_rect.y)
		SDL_Log("%s: y position unspecified, setting y to 0");

	if (!texture_rect.w && !texture_rect.h)
	{
		texture_rect.w = surface->w;
		texture_rect.h = surface->h;
	}
	else if (texture_rect.w)
	{
		if (texture_rect.h)
			SDL_Log("%s: width and height specified, choosing width for dimension scaling");

		texture_rect.h /= (surface->w / texture_rect.w);
	}
	else
	{
		SDL_assert(texture_rect.h && !texture_rect.w);

		texture_rect.w /= (surface->h / texture_rect.h);
	}

	SDL_FreeSurface(surface);
}


void night::image::render() const
{
	SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
}

SDL_Texture const* night::image::get_texture() const
{
	return texture;
}

SDL_Rect const& night::image::get_texture_rect() const
{
	return texture_rect;
}

void night::image::set_texture_rect(SDL_Rect const& _texture_rect)
{
	if (!_texture_rect.x)
		texture_rect.x = _texture_rect.x;

	if (!_texture_rect.y)
		texture_rect.y = _texture_rect.y;

	if (_texture_rect.w)
	{
		if (texture_rect.h)
			SDL_Log("%s: width and height specified, choosing width for dimension scaling");

		texture_rect.h /= (_texture_rect.w / texture_rect.w);
	}
	else if (_texture_rect.h)
	{
		texture_rect.w /= (_texture_rect.h / texture_rect.h);
	}
}
