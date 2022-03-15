#include <SDL.h>
#include <SDL_ttf.h>

#include "base.hpp"
#include "tile.hpp"
#include "screen.hpp"
#include "sdl2.hpp"

#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "error - failed to initialize SDL\n" << SDL_GetError();
		return 1;
	}
	if (TTF_Init() == -1)
	{
		std::cout << "error - failed to initialize TTF\n" << TTF_GetError();
		return 1;
	}

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

		Screen::get().clear();
		


		std::string text_msg = "Gold: " + std::to_string(Base::get().gold);
		Screen::get().text(text_msg, sdl2::clr_black, sdl2::str_brygada, 24, 20, 20, sdl2::TTF_Align::RIGHT);
		
		text_msg = "Wheat: " + std::to_string(Base::get().gold);
		Screen::get().text(text_msg, sdl2::clr_black, sdl2::str_brygada, 24, 20, 60, sdl2::TTF_Align::RIGHT);

		text_msg = "Wood: " + std::to_string(Base::get().gold);
		Screen::get().text(text_msg, sdl2::clr_black, sdl2::str_brygada, 24, 20, 100, sdl2::TTF_Align::RIGHT);

		text_msg = "Gems: " + std::to_string(Base::get().gold);
		Screen::get().text(text_msg, sdl2::clr_black, sdl2::str_brygada, 24, 20, 140, sdl2::TTF_Align::RIGHT);
		
		

		Screen::get().update();
	}

END_SDL:
	return 0;
}
