#include "scene.hpp"
#include "base.hpp"
#include "battle.hpp"

#include "SDL.h"

#ifdef _WIN32
	#include <windows.h>
#elif __APPLE__
	#include <ApplicationServices/ApplicationServices.h>
#elif __linux__
	#include <X11/Xlib.h>
#endif

int get_screen_width()
{
#ifdef _WIN32
	return GetSystemMetrics(SM_CXSCREEN);
#elif __APPLE__
	CGDirectDisplayID main_display = CGMainDisplayID();
	return CGDisplayPixelsWide(mainDisplay);
#elif __linux__
	Display* display = XOpenDisplay(nullptr);
	if (display)
	{
		int width = DefaultScreenOfDisplay(display)->width;
		XCloseDisplay(display);
		return width;
	}
	else
	{
		return -1;
	}
#endif
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	// Get width of device, set that as width of window, then using 16:9 ration get height of window
	int screen_width = get_screen_width();
	if (screen_width == -1)
	{
		SDL_Log("Failed to get screen metrics for Linux device\n");
		SDL_Quit();
		return 1;
	}
	int screen_height = (9 * screen_width) / 16;

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_CreateWindowAndRenderer(screen_width, screen_height, SDL_WINDOW_SHOWN, &window, &renderer);
	if (!window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	if (!renderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


	Base base(window, renderer, screen_width, screen_height);
	Battle battle(window, renderer, screen_width, screen_height);

	Scene::current_scene = &base;
	Scene::scenes[0] = &base;
	Scene::scenes[1] = &battle;


	SDL_Event event;

	enum class MouseState {
		Clicked,
		Dragged,
		NONE
	} mouse_state = MouseState::NONE;

	Uint32 mouse_down_time;


	while (true)
	{
		SDL_PumpEvents();

		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
			{
				mouse_down_time = SDL_GetTicks();
				mouse_state = MouseState::Clicked;

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (mouse_state == MouseState::Clicked)
					Scene::current_scene->handle_mouse_click(mouse_x, mouse_y);
				else if (mouse_state == MouseState::Dragged)
					Scene::current_scene->handle_mouse_drag_end(mouse_x, mouse_y);

				mouse_state = MouseState::NONE;

				break;
			}

			case SDL_MOUSEWHEEL:
				Scene::current_scene->handle_scroll(event.wheel.y);
				break;

			case SDL_QUIT:
				goto END_GAME_LOOP;
			}
		}

		// Turn a mouse press into a mouse drag if the mouse is held down for
		// more than 200 ms.
		if (mouse_state == MouseState::Clicked &&
			SDL_GetTicks() - mouse_down_time > 200)
		{
			Scene::current_scene->handle_mouse_drag_start(mouse_x, mouse_y);
			mouse_state = MouseState::Dragged;
		}

		if (mouse_state == MouseState::Dragged)
			Scene::current_scene->handle_mouse_drag_duration(mouse_x, mouse_y);

		Scene::current_scene->update();
		Scene::current_scene->render();

		SDL_RenderPresent(renderer);
	}

END_GAME_LOOP:;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
