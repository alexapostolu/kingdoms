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

	int b = SDL_GetTicks();
	while (true)
	{
	    int d = SDL_GetTicks() - b;
	    std::cout << d
		

	    if (delta > 1000/60.0)
	    {
		std::cout << "fps: " << 1000 / d << '\n';
	        b = SDL_GetTicks();
	    }
		
		
		Screen::get().clear();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN: {
				if (event.button.button != SDL_BUTTON_LEFT)
					break;

				int x, y;
				SDL_GetMouseState(&x, &y);

				std::cout << "mouse pos: " << x << ' ' << y << '\n';

				if (Base::get().mouse_on_shop(x, y))
				{
					Base::get().shop_state = Base::ShopState::APPEARING;
				}

				break;
			}
			case SDL_QUIT:
				goto END_SDL;
			default:
				break;
			}
		}

		

		Base::get().display_resources();
		Base::get().display_shop();



		Screen::get().update();
	}

END_SDL:
	return 0;
}
