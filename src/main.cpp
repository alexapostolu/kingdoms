#include "SDL.h"
#include "SDL_image.h"

#include <Windows.h> // GetSystemMetrics
#include <iostream>
#include <cmath>

// x += (target - x) * 0.1

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
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = (9 * screen_width) / 16;

	// Create an SDL window
	SDL_Window* window = SDL_CreateWindow("Kingdoms",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height,
		SDL_WINDOW_SHOWN);

	if (!window)
	{
		SDL_Log("Could not create window: %s", SDL_GetError());
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

	// Load image using SDL_image
	SDL_Surface* surface = IMG_Load("../../assets/farmhouse.png");
	if (!surface)
	{
		SDL_Log("Failed to load image: %s\n", IMG_GetError());
		return 1;
	}

	int farmhouse_width = surface->w;
	int farmhouse_height = surface->h;

	// Convert surface to texture
	SDL_Texture* farmhouse = SDL_CreateTextureFromSurface(renderer, surface);
	if (!farmhouse)
	{
		SDL_Log("Failed to create texture: %s\n", SDL_GetError());
		return 1;
	}
	SDL_FreeSurface(surface);

	// for resource gathering
	//SDL_TimerID timerID = SDL_AddTimer(1000, callback, "SDL");

	// Main loop
	float positions[2][2] = { { 305, 200 }, { 500, 600 } };
	float scale = 1;
	SDL_Event event;
	bool mouse_down = false;
	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;
	bool running = true;
	SDL_Vertex vertex_1 = { {10, 0}, {255, 0, 0, 255}, {1, 1} };
	SDL_Vertex vertex_2 = { {20.5, 10.5}, {255, 0, 0, 255}, {1, 1} };
	SDL_Vertex vertex_3 = { {10.5, 20.5}, {255, 0, 0, 255}, {1, 1} };
	float grid[40][30][2];
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 40; ++j) {
			grid[j][i][0] = (j + 0.5) * 35;
			grid[j][i][1] = (i + 0.5) * 25;
		}
	}
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

				if (new_scale < 0.5f || new_scale > 6.f)
					break;

				// Handle position offset from scroll
				for (int i = 0; i < 2; ++i)
				{
					// Find offset from position to mouse
					// Multiply that by new scale to get new offset
					// Divide by old scale to get relative offset?
					// Update the new position to be from the offset of mouse
					// (The center of the scale is the mouse, so it makes sense for the scale to be at the mouse)
					positions[i][0] = (positions[i][0] - event.wheel.mouseX) * new_scale / scale + event.wheel.mouseX;
					positions[i][1] = (positions[i][1] - event.wheel.mouseY) * new_scale / scale + event.wheel.mouseY;
				}

				for (int i = 0; i < 30; ++i)
				{
					for (int j = 0; j < 40; ++j)
					{
						grid[j][i][0] = (grid[j][i][0] - event.wheel.mouseX) * new_scale / scale + event.wheel.mouseX;
						grid[j][i][1] = (grid[j][i][1] - event.wheel.mouseY) * new_scale / scale + event.wheel.mouseY;
					}
				}

				scale = new_scale;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				mouse_down = true;
				drag_start_x = event.button.x;
				drag_start_y = event.button.y;
			}
			if (event.type == SDL_MOUSEBUTTONUP && mouse_down)
			{
				mouse_down = false;
			}
			if (event.type == SDL_MOUSEMOTION && mouse_down)
			{
				int delta_x = event.motion.x - drag_start_x;
				int delta_y = event.motion.y - drag_start_y;

				if (end_drag_x + delta_x < -screen_width / 2 || end_drag_x + delta_x > screen_width / 2)
					break;
				if (end_drag_y + delta_y < -screen_height / 2 || end_drag_y + delta_y > screen_height / 2)
					break;

				end_drag_x += delta_x;
				end_drag_y += delta_y;


				for (int i = 0; i < 2; ++i)
				{
					positions[i][0] += delta_x;
					positions[i][1] += delta_y;
				}

				for (int i = 0; i < 30; ++i)
				{
					for (int j = 0; j < 40; ++j)
					{
						grid[j][i][0] += delta_x;
						grid[j][i][1] += delta_y;
					}
				}

				drag_start_x = event.motion.x;
				drag_start_y = event.motion.y;
			}
		}

		// Render your frame here

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (int i = 0; i < 30; ++i)
		{
			for (int j = 0; j < 40; ++j)
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

		float w = 200 * scale;
		float h = farmhouse_height / (farmhouse_width / 200) * scale;

		for (int i = 0; i < 2; ++i)
		{
			SDL_FRect dest_rect = {
				positions[i][0] - w / 2.0,
				positions[i][1] - h / 2.0,
				w, h
			};
			SDL_RenderCopyF(renderer, farmhouse, NULL, &dest_rect);
		}

		SDL_RenderPresent(renderer);

		// Delay to limit frame rate
		SDL_Delay(16);
	}

	//SDL_RemoveTimer(timerID);
	SDL_DestroyTexture(farmhouse);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
