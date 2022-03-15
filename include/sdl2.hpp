#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>

namespace sdl2
{

// smart pointer wrappers

struct SDL_Deleter
{
	void operator()(SDL_Window*   ptr) { if (ptr) SDL_DestroyWindow(ptr); }
	void operator()(SDL_Renderer* ptr) { if (ptr) SDL_DestroyRenderer(ptr); }
	void operator()(SDL_Surface*  ptr) { if (ptr) SDL_FreeSurface(ptr); }
	void operator()(SDL_Texture*  ptr) { if (ptr) SDL_DestroyTexture(ptr); }

	void operator()(TTF_Font* ptr) { if (ptr) TTF_CloseFont(ptr); }
};

using window_ptr = std::unique_ptr<SDL_Window, SDL_Deleter>;
using renderer_ptr = std::unique_ptr<SDL_Renderer, SDL_Deleter>;
using surface_ptr  = std::unique_ptr<SDL_Surface,  SDL_Deleter>;
using texture_ptr  = std::unique_ptr<SDL_Texture,  SDL_Deleter>;

using font_ptr = std::unique_ptr<TTF_Font, SDL_Deleter>;
	



// text fonts, colours, and alignment

enum class TTF_Align
{
	LEFT, CENTER, RIGHT
};

std::string const str_brygada = "../assets/brygada.ttf";
SDL_Color const clr_black{ 0, 0, 0, 255 };
SDL_Color const clr_yellow{ 255, 239, 0, 255 };
SDL_Color const clr_white{ 255, 255, 255, 255 };

}