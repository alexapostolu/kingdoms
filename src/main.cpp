#include "grid.hpp"
#include "farmhouse.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>
#include <forward_list>
#include <vector>
#include <array>

#include <math.h>

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

#define TICK_INTERVAL 30

Uint32 next_time;

Uint32 time_left(void)
{
	Uint32 now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

king::Grid grid(40, 60);

/* Scroll */

static constexpr float MIN_SCALE = 0.5f;
static constexpr float MAX_SCALE = 6.f;

int main(int argc, char* argv[])
{
	// Initialize SDL
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

	float scale = 1;

	std::forward_list<king::Farmhouse> farmhouses{
		king::Farmhouse(renderer, { 300, 200 }, grid, scale),
		king::Farmhouse(renderer, { 600, 300 }, grid, scale)
	};

	for (auto& farmhouse : farmhouses)
		farmhouse.init_resource_timer();

	bool mouse_down = false;
	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;
	bool running = true;

	next_time = SDL_GetTicks() + TICK_INTERVAL;

	Uint32 mouse_press_time = 0;

	bool mouse_in_motion = false;

	int stop_drag = 0;

	SDL_Event event;

	// Main loop
	while (running)
	{
		// Pump the event loop
		SDL_PumpEvents();

		// Poll for events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
				break;
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				// Handle scroll
				int scroll = event.wheel.y;
				float new_scale = scale + scroll * 0.1f;

				/*if (new_scale < MIN_SCALE)
					lerp_scroll = LerpScroll::IN_;
				else if (new_scale > MAX_SCALE)
					lerp_scroll = LerpScroll::OUT_;*/

				printf("%f\n", new_scale);

				float scale_ratio = new_scale / scale;
				float mouse_x = event.wheel.mouseX;
				float mouse_y = event.wheel.mouseY;

				grid.mouse_wheel(mouse_x, mouse_y, scale_ratio);

				for (auto& farmhouse : farmhouses)
					farmhouse.mouse_wheel(mouse_x, mouse_y, scale_ratio);

				scale = new_scale;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (stop_drag != 0)
					break;

				mouse_press_time = SDL_GetTicks();

				mouse_down = true;
				drag_start_x = event.button.x;
				drag_start_y = event.button.y;
			}
			else if (event.type == SDL_MOUSEBUTTONUP && mouse_down)
			{
				mouse_in_motion = false;
				mouse_down = false;
				king::Farmhouse::drag_ptr = nullptr;

				for (auto& farmhouse : farmhouses)
					farmhouse.mouse_release();
			}
			else if (event.type == SDL_MOUSEMOTION && mouse_down)
			{
				mouse_in_motion = true;

				if (king::Farmhouse::drag_ptr)
				{
					king::Farmhouse::drag_ptr->mouse_drag(event.motion.x, event.motion.y, farmhouses, scale);
				}
				else
				{
					float delta_x = event.motion.x - drag_start_x;
					float delta_y = event.motion.y - drag_start_y;

					if (end_drag_x + delta_x < -screen_width / 2)
						stop_drag = 1;
					if (end_drag_x + delta_x > screen_width / 2)
						stop_drag = -1;
					if (end_drag_y + delta_y < -screen_height / 2)
						stop_drag = 2;
					if (end_drag_y + delta_y > screen_height / 2)
						stop_drag = -2;

					end_drag_x += delta_x;
					end_drag_y += delta_y;

					grid.mouse_drag(delta_x, delta_y);

					for (auto& farmhouse : farmhouses)
						farmhouse.pan(delta_x, delta_y);

					drag_start_x = event.motion.x;
					drag_start_y = event.motion.y;
				}
			}
		}

		if (stop_drag != 0)
		{
			if (stop_drag == 1)
			{
				if (end_drag_x >= -screen_width / 2)
				{
					stop_drag = 0;
				}
				else if (end_drag_x < -screen_width / 2)
				{
					float dist_remaining = (end_drag_x)-(-screen_width / 2);
					grid.mouse_drag(-dist_remaining * 0.1, 0);
					for (auto& farmhouse : farmhouses)
						farmhouse.pan(-dist_remaining * 0.1, 0);
					end_drag_x -= dist_remaining * 0.1;
				}
			}
			else if (stop_drag == -1)
			{
				if (end_drag_x <= screen_width / 2)
				{
					stop_drag = 0;
				}
				else if (end_drag_x > screen_width / 2)
				{
					float dist_remaining = -(end_drag_x)+(screen_width / 2);
					grid.mouse_drag(dist_remaining * 0.1, 0);
					for (auto& farmhouse : farmhouses)
						farmhouse.pan(dist_remaining * 0.1, 0);
					end_drag_x += dist_remaining * 0.1;
				}
			}

			if (stop_drag == 2)
			{
				if (end_drag_y >= -screen_height/ 2)
				{
					stop_drag = 0;
				}
				else if (end_drag_y < -screen_height/ 2)
				{
					float dist_remaining = (end_drag_y)-(-screen_height / 2);
					grid.mouse_drag(0, -dist_remaining * 0.1);
					for (auto& farmhouse : farmhouses)
						farmhouse.pan(0, -dist_remaining * 0.1);
					end_drag_y -= dist_remaining * 0.1;
				}
			}
			else if (stop_drag == -2)
			{
				if (end_drag_y <= screen_height/ 2)
				{
					stop_drag = 0;
				}
				else if (end_drag_y > screen_height / 2)
				{
					float dist_remaining = -(end_drag_y)+(screen_height / 2);
					grid.mouse_drag(0, dist_remaining * 0.1);
					for (auto& farmhouse : farmhouses)
						farmhouse.pan(0, dist_remaining * 0.1);
					end_drag_y += dist_remaining * 0.1;
				}
			}
		}

		// Handle mouse drag building
		if (SDL_GetTicks() - mouse_press_time >= 200 && mouse_down && !mouse_in_motion && !king::Farmhouse::drag_ptr)
		{
			// If mouse has been pressed for 200 ms, then assume player is trying to select a building

			int mx, my;
			SDL_GetMouseState(&mx, &my);

			for (auto& farmhouse : farmhouses)
			{
				if (farmhouse.mouse_press(mx, my))
					break;
			}

			mouse_press_time = SDL_GetTicks();
		}

		// Render frame

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// Draw grid
		grid.render(renderer, scale);

		for (auto& farmhouse : farmhouses)
			farmhouse.render(renderer, scale);

		SDL_RenderPresent(renderer);

		// Delay to limit frame rate
		SDL_Delay(time_left());	
		next_time += TICK_INTERVAL;
	}

	for (auto& farmhouse : farmhouses)
		SDL_DestroyTexture(farmhouse.texture);

	//SDL_RemoveTimer(timerID); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
