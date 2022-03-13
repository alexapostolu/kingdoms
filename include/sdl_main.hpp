#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>

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