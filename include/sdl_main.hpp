#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>

struct SDL_Deleter
{
	void operator()(SDL_Window*   ptr) { if (ptr) SDL_DestroyWindow(ptr); }
	void operator()(SDL_Renderer* ptr) { if (ptr) SDL_DestroyRenderer(ptr); }
	void operator()(SDL_Surface*  ptr) { if (ptr) SDL_FreeSurface(ptr); }
	void operator()(SDL_Texture*  ptr) { if (ptr) SDL_DestroyTexture(ptr); }
};

using WindowPtr   = std::unique_ptr<SDL_Window,   SDL_Deleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SDL_Deleter>;
using SurfacePtr  = std::unique_ptr<SDL_Surface,  SDL_Deleter>;
using TexturePtr  = std::unique_ptr<SDL_Texture,  SDL_Deleter>;

class SDL_Main
{
public:
	void text(std::string_view text, TTF_Font const& font, TTF_Color const& colour)
	{
		SDL_Surface* text_surface = TTF_RenderText_Solid(font, text.c_str(), colour);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	
		int text_w, text_h;
		int s = TTF_SizeText(ttf_brygada, text_message.c_str(), &text_w, &text_h);
		SDL_Rect text_rect{ (SCREEN_WIDTH - text_w) - 20, 20, text_w, text_h };

		SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);
	}

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
}
