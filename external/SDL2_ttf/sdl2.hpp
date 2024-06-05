#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>
#include <random>

namespace sdl2
{

// smart pointer wrappers

struct SDL_Deleter
{
	void operator()(SDL_Renderer* ptr);
	void operator()(SDL_Surface* ptr);
	void operator()(SDL_Texture* ptr);
	void operator()(TTF_Font* ptr);
};

using window_ptr = std::shared_ptr<SDL_Window>;
using renderer_ptr = std::unique_ptr<SDL_Renderer, SDL_Deleter>;
using surface_ptr  = std::unique_ptr<SDL_Surface,  SDL_Deleter>;
using texture_ptr  = std::unique_ptr<SDL_Texture,  SDL_Deleter>;

using font_ptr = std::unique_ptr<TTF_Font, SDL_Deleter>;




// inclusive
int rand_int(int const lb, int const ub);
// inclusive, 1 decial place
double rand_dbl(double const lb, double const ub);


// text fonts, colours, and alignment

enum class LineMode
{
	ALIASING,
	ANTIALIASING
};

enum class TrigAlign
{
	CENTER,
	CORNERS
};

enum class RectAlign
{
	CORNERS,
	CENTER,
	CENTER_LEFT,
	CENTER_RIGHT
};

using ImageAlign = RectAlign;
using TextAlign = RectAlign;

enum class CircleQuad
{
	ALL,
	TOP_LEFT, TOP_RIGHT,
	BOTTOM_LEFT, BOTTOM_RIGHT
};

// stroke quad for triangle
// if two sides are both top, then the left one is top and the right one is middle
enum class TrigQuad
{
	ALL,
	TOP,
	MIDDLE,
	BOTTOM
};

struct Dimension
{
	int x, y, w, h;
};

struct Text
{
	Text(sdl2::renderer_ptr& renderer, std::string const& _text, int _x, int _y, SDL_Color _clr, std::string const& _font, int _size, TextAlign _align);
	bool clicked_on(int mx, int my) const;

	std::string text;
	Dimension dim;
	SDL_Color clr;
	std::string font;
	int size;
	RectAlign align;

	sdl2::texture_ptr texture;
};

std::string const str_brygada = "../assets/brygada.ttf";
SDL_Color const clr_black{ 0, 0, 0, 255 };
SDL_Color const clr_yellow{ 255, 239, 0, 255 };
SDL_Color const clr_green{ 60, 220, 0, 150 };
SDL_Color const clr_white{ 255, 255, 255, 255 };
SDL_Color const clr_clear{ 0, 0, 0, 0 };
SDL_Color const clr_red{ 255, 50, 50, 150 };
SDL_Color const clr_gray{ 240, 240, 240, 170 };

}