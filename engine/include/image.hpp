#pragma once

#include "SDL.h"

#include <string>


namespace night {

/*
 * The image class initializes an texture and provides functionality for the
 * position, dimension and rendering of the texture.
 */
class image
{
public:
	/*
	 * @param _renderer		Renderer to display the image.
	 * @param image_path	Relative path to image file.
	 */
	image(
		std::string const& image_path,
		SDL_Renderer* _renderer,
		SDL_Rect const& _texture_rect
	);

public:
	/*
	 * Render the texture using the position and dimension in texture_rect.
	 */
	void render() const;

public:
	SDL_Texture const* get_texture() const;

	SDL_Rect const& get_texture_rect() const;

	/*
	 * Set the texture rectangle (position and dimension) for rendering.
	 * 
	 * X and Y position will be updated is specified, and width and height will be
	 * scaled accordingly if specified.
	 * 
	 * @param _texture_rect		The new texture rectangle.
	 */
	void set_texture_rect(SDL_Rect const& _texture_rect);

protected:
	SDL_Renderer* renderer;

	SDL_Texture* texture;
	SDL_Rect texture_rect;
};

}; // night::
