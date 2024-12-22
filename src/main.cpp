#include "grid.hpp"
#include "resource_building.hpp"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_FontCache.h"

#include <iostream>
#include <forward_list>
#include <bitset>
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

Grid grid(40, 60);

/* Scroll */

static constexpr float MIN_SCALE = 0.5f;
static constexpr float MAX_SCALE = 6.f;

namespace MouseState {
	enum {
		PRESS = 0x00001,
		DRAG_GRID = 0x00010,
		DRAG_BUILDING = 0x00100,
		RELEASE = 0x01000,
		SCROLL = 0x10000,
		NONE = 0x00000
	};
}

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

	FC_Font* font = FC_CreateFont();
	FC_LoadFont(font, renderer, "../../assets/Cinzel.ttf", screen_height / 24, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	float scale = 1;

	std::forward_list<ResourceBuilding> resource_buildings{
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, { 300, 200 }, grid, scale, font),
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, { 600, 300 }, grid, scale, font),
		ResourceBuilding(ResourceBuildingType::LUMBERMILL, renderer, { 900, 300 }, grid, scale, font)
	};

	for (auto& resource_building : resource_buildings)
		resource_building.init_resource_timer();

	bool mouse_down = false;
	bool mouse_in_motion = false;
	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;

	int wheat = 0;
	int wood = 0;

	Uint32 mouse_press_time = 0;

	int stop_drag = 0;

	// Handle mouse events (scroll, press and drag)
	int mouse_state = MouseState::NONE;
	Uint32 mouse_time = SDL_GetTicks();
	int mouse_scroll;

	SDL_Rect shop_bar{ 0, screen_height - 250, screen_width, 300 };
	bool display_shop = false;

	std::forward_list<ResourceBuilding> shop_resource_buildings{
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, SDL_FPoint{ 120, (float)shop_bar.y + 100 }, grid, scale, font),
		ResourceBuilding(ResourceBuildingType::LUMBERMILL, renderer, SDL_FPoint{ 400, (float)shop_bar.y + 100 }, grid, scale, font)
	};

	bool new_building = false;

	bool game_loop = true;
	SDL_Event event;

	// Main loop
	while (game_loop)
	{
		// Pump the event loop
		SDL_PumpEvents();

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
				mouse_down = true;
				mouse_time = SDL_GetTicks();

				for (auto& resource_building : resource_buildings)
				{
					if (resource_building.mouse_press(event.button.x, event.button.y))
					{
						ResourceBuilding::drag_ptr = &resource_building;
						break;
					}
				}

				break;

			case SDL_MOUSEBUTTONUP:
				mouse_down = false;
				mouse_state |= MouseState::RELEASE;
				mouse_state &= ~MouseState::DRAG_GRID; // Mouse up signals end of drag
				mouse_state &= ~MouseState::DRAG_BUILDING;
				mouse_in_motion = false;

				if (SDL_GetTicks() - mouse_time <= 200) // <= 200 ms for mouse press
					mouse_state |= MouseState::PRESS;

				break;

			case SDL_MOUSEWHEEL:
				mouse_state |= MouseState::SCROLL;
				mouse_scroll = event.wheel.y;
				break;

			case SDL_QUIT:
				game_loop = false;
				break;
			}

			// Ensure mouse drag is only assigned once so start mouse position isn't overriden
			if (mouse_down && SDL_GetTicks() - mouse_time > 200 && !mouse_in_motion) // > 200 ms for mouse drag
			{
				if (!ResourceBuilding::drag_ptr)
				{
					mouse_in_motion = true;
					mouse_state |= MouseState::DRAG_GRID;
					drag_start_x = event.button.x;
					drag_start_y = event.button.y;
				}
				else if (SDL_GetTicks() - mouse_time > 400)
				{
					mouse_in_motion = true;
					mouse_state |= MouseState::DRAG_BUILDING;
					drag_start_x = event.button.x;
					drag_start_y = event.button.y;
				}
			}
		}

		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);

		if (mouse_state & MouseState::SCROLL)
		{
			float new_scale = scale + mouse_scroll * 0.1f;
			if (new_scale >= MIN_SCALE && new_scale <= MAX_SCALE)
			{
				float scale_ratio = new_scale / scale;
				grid.mouse_wheel(mouse_x, mouse_y, scale_ratio);

				for (auto& resource_building : resource_buildings)
				{
					resource_building.mouse_wheel(mouse_x, mouse_y, scale_ratio);
				}

				scale = new_scale;
			}

			// Mouse scroll is a one time thing, so we clear mouse scroll after this
			mouse_state &= ~MouseState::SCROLL;
		}

		if (mouse_state & MouseState::PRESS)
		{
			// Collect resource from resource_building if clicked on
			for (auto& resource_building : resource_buildings)
			{
				if (resource_building.mouse_press(mouse_x, mouse_y))
				{
					switch (resource_building.type)
					{
					case ResourceBuildingType::FARMHOUSE:
						wheat += resource_building.mouse_press_update(scale);
						break;
					case ResourceBuildingType::LUMBERMILL:
						wood += resource_building.mouse_press_update(scale);
						break;
					}

					// Resource buildings cannot overlap
					break;
				}
			}

			if (sqrt(pow(screen_width - 200 - mouse_x, 2) + pow(screen_height - 100 - mouse_y, 2)) < 100)
				display_shop = true;
			if (sqrt(pow(screen_width - 200 - mouse_x, 2) + pow(screen_height - 300 - mouse_y, 2)) < 100)
				display_shop = false;

			// Mouse press is a one time thing, so we clear mouse press after this
			mouse_state &= ~MouseState::PRESS;
		}

		if (mouse_state & MouseState::DRAG_GRID)
		{
			if (display_shop)
			{
				for (auto& shop_resource_building : shop_resource_buildings)
				{
					if (shop_resource_building.mouse_press(mouse_x, mouse_y))
					{
						// Create new resource building
						ResourceBuilding new_resource_building(
							shop_resource_building.type,
							renderer, SDL_FPoint{ (float)mouse_x, (float)mouse_y }, grid, scale,
							font);

						// Add it to resource buildings
						resource_buildings.push_front(new_resource_building);
						resource_buildings.front().init_resource_timer();
						ResourceBuilding::drag_ptr = &resource_buildings.front();

						// Update shop and mouse state
						display_shop = false;
						mouse_state &= ~MouseState::DRAG_GRID;
						mouse_state |= MouseState::DRAG_BUILDING;
						new_building = true;

						break;
					}
				}
			}
			else
			{
				float delta_x = mouse_x - drag_start_x;
				float delta_y = mouse_y - drag_start_y;

				if (end_drag_x + delta_x < -screen_width / 2)
					stop_drag = 1;
				else if (end_drag_x + delta_x > screen_width / 2)
					stop_drag = -1;
				else if (end_drag_y + delta_y < -screen_height / 2)
					stop_drag = 2;
				else if (end_drag_y + delta_y > screen_height / 2)
					stop_drag = -2;

				end_drag_x += delta_x;
				end_drag_y += delta_y;

				grid.mouse_drag(delta_x, delta_y);

				for (auto& resource_building : resource_buildings)
					resource_building.pan(delta_x, delta_y);

				drag_start_x = mouse_x;
				drag_start_y = mouse_y;
			}
		}

		if (mouse_state & MouseState::DRAG_BUILDING)
		{
			float delta_x = mouse_x - drag_start_x;
			float delta_y = mouse_y - drag_start_y;

			ResourceBuilding::drag_ptr->mouse_drag(delta_x, delta_y, resource_buildings, scale);
			drag_start_x = mouse_x;
			drag_start_y = mouse_y;
		}

		if (mouse_state & MouseState::RELEASE)
		{
			if (ResourceBuilding::drag_ptr)
			{
				bool is_blocked = ResourceBuilding::drag_ptr->mouse_release();
				if (is_blocked && new_building)
					resource_buildings.pop_front();

				ResourceBuilding::drag_ptr = nullptr;
				new_building = false;
			}

			mouse_state &= ~MouseState::RELEASE;
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
					for (auto& resource_building : resource_buildings)
						resource_building.pan(-dist_remaining * 0.1, 0);
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
					for (auto& resource_building : resource_buildings)
						resource_building.pan(dist_remaining * 0.1, 0);
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
					for (auto& resource_building : resource_buildings)
						resource_building.pan(0, -dist_remaining * 0.1);
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
					for (auto& resource_building : resource_buildings)
						resource_building.pan(0, dist_remaining * 0.1);
					end_drag_y += dist_remaining * 0.1;
				}
			}
		}

		// Render frame

		SDL_SetRenderDrawColor(renderer, 82, 166, 84, 255);
		SDL_RenderClear(renderer);

		// Draw grid
		grid.render(renderer, scale);

		for (auto& resource_building : resource_buildings)
		{
			resource_building.update();
			resource_building.render(renderer, scale);
		}

		// Resource bar
		SDL_Rect resource_bar{ 0, 0, screen_width, screen_height / 12 };
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, &resource_bar);
		FC_Draw(font, renderer, 0, screen_height / 24 - screen_height / 48, "Wheat: %d", wheat);
		FC_Draw(font, renderer, 300, screen_height / 24 - screen_height / 48, "Wood: %d", wood);

		// Shop
		if (!display_shop)
		{
			FC_DrawColor(font, renderer, screen_width - 200, screen_height - 100, SDL_Colour{0, 0, 0, 255}, "Shop");
		}
		else
		{
			FC_DrawColor(font, renderer, screen_width - 200, screen_height - shop_bar.h - 20, SDL_Colour{ 0, 0, 0, 255 }, "Close");
			SDL_RenderFillRect(renderer, &shop_bar);

			for (auto& shop_resource_building : shop_resource_buildings)
				shop_resource_building.render(renderer, 1);
		}

		SDL_RenderPresent(renderer);
	}

	for (auto& resource_building : resource_buildings)
		SDL_DestroyTexture(resource_building.texture);

	//SDL_RemoveTimer(timerID); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	FC_FreeFont(font);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
