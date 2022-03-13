#include <SDL.h>
#include <SDL_ttf.h>

#include "base.hpp"
#include "tile.hpp"
#include "sdl_main.hpp"

#include <iostream>
#include <memory>
#include <string>

constexpr int SCREEN_WIDTH = 1170,
			  SCREEN_HEIGHT = 525;

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "error - failed to initialize SDL\n" << SDL_GetError();
		return 1;
	}

	std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
		SDL_CreateWindow("Nighthawk - Kingdoms",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH, SCREEN_HEIGHT, 0),
		SDL_DestroyWindow);

	if (!window)
	{
		std::cout << "error - failed to open window\n" << SDL_GetError();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		std::cout << "error - failed to create renderer\n" << SDL_GetError();
		return 1;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "error - failed to initialize TTF\n" << TTF_GetError();
		return 1;
	}

	TTF_Font* ttf_brygada = TTF_OpenFont("../assets/brygada.ttf", 24);
	SDL_Color clr_black{ 0, 0, 0 };

	while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					goto END_SDL;
				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
		SDL_RenderClear(renderer);

		std::string text_message = "Gold: " + std::to_string(Base::get().gold);
		SDL_Surface* text_surface = TTF_RenderText_Solid(ttf_brygada, text_message.c_str(), clr_black);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		int text_w, text_h;
		int s = TTF_SizeText(ttf_brygada, text_message.c_str(), &text_w, &text_h);
		SDL_Rect text_rect{ (SCREEN_WIDTH - text_w) - 20, 20, text_w, text_h };

		SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);



		SDL_RenderPresent(renderer);
	}

END_SDL:;
}