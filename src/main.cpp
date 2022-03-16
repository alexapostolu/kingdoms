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
			case SDL_MOUSEBUTTONDOWN: {

				if (event.button.button != SDL_BUTTON_RIGHT)
					break;

				int x, y;
				SDL_GetMouseState(&x, &y);

				std::cout << x << ' ' << y << '\n';

				break;
			}
			case SDL_QUIT:
				goto END_SDL;
			default:
				break;
			}
		}

		Screen::get().clear();

		

		Base::get().display_resources();
		Base::get().display_shop();



		Screen::get().update();
	}

END_SDL:
	return 0;
}
