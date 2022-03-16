#pragma once

#include "sdl2.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <memory>

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
	void text(std::string const& text, SDL_Color const& colour, std::string const& font, int size,
		int x, int y, sdl2::TTF_Align alignment);
	void image(std::string const& file, int x, int y, int width, int height);

public:
	int const SCREEN_WIDTH,
			  SCREEN_HEIGHT;

private:
	Screen();

private:
	sdl2::window_ptr window;
	sdl2::renderer_ptr renderer;
};