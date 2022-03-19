#pragma once

#include "sdl2.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <memory>
#include <unordered_map>

class Screen
{
public:
	static Screen& get()
	{
		static Screen instance;
		return instance;
	}

public:
	Screen(Screen const&) = delete;
	void operator=(Screen const&) = delete;

public:
	void update();
	void clear();

	void rect(int x, int y, int width, int height, SDL_Color colour);
	// x, y is where the text starts, align is the direction of how the text is written
	void text(std::string const& text, SDL_Color const& colour, std::string const& font, int size,
		int x, int y, sdl2::TTF_Align alignment);
	void image_store(std::string const& img);
	void image_display(std::string const& img, int x, int y, int w, int h);

public:
	int const SCREEN_WIDTH,
			  SCREEN_HEIGHT;

private:
	Screen();

private:
	sdl2::window_ptr window;
	sdl2::renderer_ptr renderer;

	std::unordered_map<std::string, sdl2::texture_ptr> images;
};