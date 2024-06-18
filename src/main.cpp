#include "object.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>
#include <forward_list>
#include <vector>

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
		Screen* screen = DefaultScreenOfDisplay(display);
		metrics.width = screen->width;
		metrics.height = screen->height;
		XCloseDisplay(display);
	}
	else
	{
		return -1;
	}
#endif
}

// x += (target - x) * 0.1

float dist(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

int main(int argc, char* argv[])
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	// SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);

	// Get width of device, set that as width of window, then using 16:9 ration get height of window
	int screen_width = get_screen_width();
	if (screen_width == -1)
	{
		SDL_Log("Failed to get screen metrics for Linux device\n");
		SDL_Quit();
		return 1;
	}
	int screen_height = (9 * screen_width) / 16;

	// Create an SDL window
	SDL_Window* window = SDL_CreateWindow("Kingdoms",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height,
		SDL_WINDOW_SHOWN);

	if (!window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Create a renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		SDL_Log("Could not create renderer: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// for resource gathering
	//SDL_TimerID timerID = SDL_AddTimer(1000, callback, "SDL");

	// Main loop
	
	SDL_Event event;

	std::forward_list<king::Farmhouse> farmhouses;

	float scale = 1;
	bool mouse_down = false;
	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;
	bool running = true;
	int const grid_width = 40;
	int const grid_height = 60;
	float grid[grid_width][grid_height][2];
	for (int i = 0; i < grid_height; ++i)
	{
		for (int j = 0; j < grid_width; ++j)
		{
			grid[j][i][0] = j * 35 + (i % 2 == 0 ? 0 : 17.5);
			grid[j][i][1] = i * 12.5;
		}
	}

	Uint32 mouse_press_time = 0;

	// Find closest rhombus to center of farm grid. This rhombus is the center.
	int fx = 0, fy = 0;
	for (int i = 0; i < grid_height; ++i)
	{
		for (int j = 0; j < grid_width; ++j)
		{
			if (dist(grid[j][i][0], grid[j][i][1], 305, 200) <
				dist(grid[fx][fy][0], grid[fx][fy][1], 305, 200))
			{
				fx = j;
				fy = i;
			}
		}
	}


	farmhouses.push_front(king::Farmhouse(
		renderer,
		std::array<SDL_Vertex, 4>{
			SDL_Vertex{ grid[fx + 2][fy + 3][0] + 17.5f, grid[fx + 2][fy + 3][1] },
			SDL_Vertex{ grid[fx][fy + 8][0] - 17.5f, grid[fx][fy + 8][1] + 25 },
			SDL_Vertex{ grid[fx][fy - 2][0] - 17.5f, grid[fx][fy - 2][1] - 25 },
			SDL_Vertex{ grid[fx - 3][fy + 3][0] - 52.5f, grid[fx - 3][fy + 3][1] }
		}
	));

	// Find closest rhombus to center of farm grid. This rhombus is the center.
	fx = 0, fy = 0;
	for (int i = 0; i < grid_height; ++i)
	{
		for (int j = 0; j < grid_width; ++j)
		{
			if (dist(grid[j][i][0], grid[j][i][1], 605, 300) <
				dist(grid[fx][fy][0], grid[fx][fy][1], 605, 300))
			{
				fx = j;
				fy = i;
			}
		}
	}

	farmhouses.push_front(king::Farmhouse(
		renderer,
		std::array<SDL_Vertex, 4>{
			SDL_Vertex{ grid[fx + 2][fy + 3][0] + 17.5f, grid[fx + 2][fy + 3][1] },
			SDL_Vertex{ grid[fx][fy - 2][0], grid[fx][fy - 2][1] - 12.5f },
			SDL_Vertex{ grid[fx][fy + 8][0], grid[fx][fy + 8][1] + 12.5f },
			SDL_Vertex{ grid[fx - 3][fy + 3][0] - 17.5f, grid[fx - 3][fy + 3][1] }
		}
	));

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
			if (event.type == SDL_MOUSEWHEEL)
			{
				// Handle scroll
				int scroll = event.wheel.y;
				float new_scale = scale + scroll * 0.1f;

				// Prevent scrolling too close or far
				if (new_scale < 0.5f || new_scale > 6.f)
					break;

				for (int i = 0; i < grid_height; ++i)
				{
					for (int j = 0; j < grid_width; ++j)
					{
						grid[j][i][0] = (grid[j][i][0] - event.wheel.mouseX) * new_scale / scale + event.wheel.mouseX;
						grid[j][i][1] = (grid[j][i][1] - event.wheel.mouseY) * new_scale / scale + event.wheel.mouseY;
					}
				}

				for (auto& farmhouse : farmhouses)
					farmhouse.zoom(event.wheel);

				scale = new_scale;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				mouse_press_time = SDL_GetTicks();

				mouse_down = true;
				drag_start_x = event.button.x;
				drag_start_y = event.button.y;

				for (auto& farmhouse : farmhouses)
					farmhouse.mouse_pressed(event.button.x, event.button.y);
			}
			if (event.type == SDL_MOUSEBUTTONUP && mouse_down)
			{
				mouse_down = false;
				king::Farmhouse::drag_ptr = nullptr;

				for (auto& farmhouse : farmhouses)
					farmhouse.mouse_released();
			}
			if (event.type == SDL_MOUSEMOTION && mouse_down && !king::Farmhouse::drag_ptr)
			{
				int delta_x = event.motion.x - drag_start_x;
				int delta_y = event.motion.y - drag_start_y;

				if (end_drag_x + delta_x < -screen_width / 2 || end_drag_x + delta_x > screen_width / 2)
					break;
				if (end_drag_y + delta_y < -screen_height / 2 || end_drag_y + delta_y > screen_height / 2)
					break;

				end_drag_x += delta_x;
				end_drag_y += delta_y;

				for (int i = 0; i < grid_height; ++i)
				{
					for (int j = 0; j < grid_width; ++j)
					{
						grid[j][i][0] += delta_x;
						grid[j][i][1] += delta_y;
					}
				}

				for (auto& farmhouse : farmhouses)
					farmhouse.pan(delta_x, delta_y);

				drag_start_x = event.motion.x;
				drag_start_y = event.motion.y;
			}
			if (event.type == SDL_MOUSEMOTION && king::Farmhouse::drag_ptr)
			{
				king::Farmhouse::drag_ptr->drag(event.motion.x, event.motion.y, farmhouses);
			}
		}

		// Handle mouse drag building
		if (SDL_GetTicks() - mouse_press_time >= 2000 && mouse_down && !king::Farmhouse::drag_ptr)
		{
			// If mouse has been pressed for 800 ms, then assume player is trying to select a building
			// Loop through all neighboring rhombuses in the grid to see if any of them are part of a building

			int mx, my;
			SDL_GetMouseState(&mx, &my);

			for (auto& farmhouse : farmhouses)
			{
				if (farmhouse.isPointInObject(mx, my))
				{
					king::Farmhouse::drag_ptr = &farmhouse;
					break;
				}
			}

			mouse_press_time = SDL_GetTicks();
		}

		// Render frame

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// Draw grid
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (int i = 0; i < grid_height; ++i)
		{
			for (int j = 0; j < grid_width; ++j)
			{
				float x = grid[j][i][0];
				float y = grid[j][i][1];
				float w = 35 * scale;
				float h = 25 * scale;
				SDL_FPoint points[5] = {
					{x,			y - h / 2},
					{x + w / 2, y		 },
					{x,			y + h / 2},
					{x - w / 2, y		 },
					{x,			y - h / 2}
				};

				SDL_RenderDrawLinesF(renderer, points, 5);
			}
		}

	/*	auto clr = farmhouses.begin()->isPolygonInObject((++farmhouses.begin())->grid_snap_vertices)
			? SDL_Colour{255, 0, 0, 255} : SDL_Colour{ 0, 255, 0, 255 };

		farmhouses.begin()->clr = clr;*/

		for (auto& farmhouse : farmhouses)
			farmhouse.render(renderer);

		SDL_RenderPresent(renderer);

		// Delay to limit frame rate
		SDL_Delay(16);
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
