#pragma once

#include "sdl2.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <optional>
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

	void rect(int x, int y, int w, int h, std::optional<SDL_Color> const& fill,
		std::optional<SDL_Color> const& stroke);
	void circle(int x, int y, int r);

	// x, y is where the text starts, align is where the text starts
	void text(std::string const& text, SDL_Color const& colour, std::string const& font, int size,
		int x, int y, sdl2::Align alignment);

	void image(std::string const& img, int x, int y, int w, int h, sdl2::Align alignment);
	void image(std::string const& img, sdl2::Dimension const& dim, sdl2::Align alignment);

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