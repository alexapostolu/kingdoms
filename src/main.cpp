#include "SDL.h"
#include "SDL_image.h"

#include <Windows.h> // GetSystemMetrics
#include <iostream>
#include <vector>

// x += (target - x) * 0.1

float dist(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// Function to calculate the cross product of two vectors (ax, ay) and (bx, by)
float crossProduct(float ax, float ay, float bx, float by) {
	return ax * by - ay * bx;
}

// Function to check if point (px, py) is inside the rhombus
bool isPointInRhombus(float px, float py, float cx, float cy, float w, float h) {
	// Define the vertices of the rhombus
	SDL_FPoint points[4] = {
		{cx,         cy - h / 2},  // Top
		{cx + w / 2, cy        },  // Right
		{cx,         cy + h / 2},  // Bottom
		{cx - w / 2, cy        }   // Left
	};

	// Calculate vectors from the vertices to the point
	float vectors[4][2] = {
		{px - points[0].x, py - points[0].y},
		{px - points[1].x, py - points[1].y},
		{px - points[2].x, py - points[2].y},
		{px - points[3].x, py - points[3].y}
	};

	// Calculate cross products for each pair of vectors
	float crossProducts[4] = {
		crossProduct(points[1].x - points[0].x, points[1].y - points[0].y, vectors[0][0], vectors[0][1]),
		crossProduct(points[2].x - points[1].x, points[2].y - points[1].y, vectors[1][0], vectors[1][1]),
		crossProduct(points[3].x - points[2].x, points[3].y - points[2].y, vectors[2][0], vectors[2][1]),
		crossProduct(points[0].x - points[3].x, points[0].y - points[3].y, vectors[3][0], vectors[3][1])
	};

	// Check if all cross products have the same sign
	bool allPositive = true;
	bool allNegative = true;

	for (int i = 0; i < 4; ++i) {
		if (crossProducts[i] < 0) allPositive = false;
		if (crossProducts[i] > 0) allNegative = false;
	}

	return allPositive || allNegative;
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
	float positions[2][2] = { { 305 / 35 * 35, 200 / 25 * 25 }, { 500 / 35 * 35, 600 / 25 * 25 } };
	float scale = 1;
	SDL_Event event;
	bool mouse_down = false;
	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;
	bool running = true;
	int const grid_width = 40;
	int const grid_height = 60;
	float grid[grid_width][grid_height][2];
	for (int i = 0; i < grid_height; ++i) {
		for (int j = 0; j < grid_width; ++j) {
			grid[j][i][0] = (j + 0) * 35 + (i % 2 == 0 ? 0 : 17.5);
			grid[j][i][1] = (i + 0) * 12.5;
		}
	}
	Uint32 mouse_press_time = 0;

	int display_farm_grid = -1;

	std::vector<std::pair<float, float>> farm_grid;
	// Find closest rhombus to center of farm grid. This rhombus is the center.
	int fx = 0, fy = 0;
	for (int i = 0; i < grid_height; ++i)
	{
		for (int j = 0; j < grid_width; ++j)
		{
			if (dist(grid[j][i][0], grid[j][i][1], positions[0][0], positions[0][1]) <
				dist(grid[fx][fy][0], grid[fx][fy][1], positions[0][0], positions[0][1]))
			{
				fx = j;
				fy = i;
			}
		}
	}


	farm_grid.push_back({ grid[fx + 3][fy + 2][0] + 17.5, grid[fx + 3][fy + 2][1] });
	//farm_grid.push_back({ grid[fx + 3][fy + 2][0], grid[fx + 3][fy + 2][1] });
	/*farm_grid.push_back({ grid[fx + 2][fy + 3][0], grid[fx + 2][fy + 3][1] });
	farm_grid.push_back({ grid[fx + 2][fy + 1][0], grid[fx + 2][fy + 1][1] });
	farm_grid.push_back({ grid[fx + 2][fy + 4][0], grid[fx + 2][fy + 4][1] });
	farm_grid.push_back({ grid[fx + 2][fy + 2][0], grid[fx + 2][fy + 2][1] });
	farm_grid.push_back({ grid[fx + 2][fy + 0][0], grid[fx + 2][fy + 0][1] });
	farm_grid.push_back({ grid[fx + 1][fy + 5][0], grid[fx + 1][fy + 5][1] });
	farm_grid.push_back({ grid[fx + 1][fy + 3][0], grid[fx + 1][fy + 3][1] });
	farm_grid.push_back({ grid[fx + 1][fy + 1][0], grid[fx + 1][fy + 1][1] });
	farm_grid.push_back({ grid[fx + 1][fy - 1][0], grid[fx + 1][fy - 1][1] });

	farm_grid.push_back({ grid[fx + 1][fy + 6][0], grid[fx + 1][fy + 6][1] });
	farm_grid.push_back({ grid[fx + 1][fy + 4][0], grid[fx + 1][fy + 4][1] });
	farm_grid.push_back({ grid[fx + 1][fy + 2][0], grid[fx + 1][fy + 2][1] });
	farm_grid.push_back({ grid[fx + 1][fy + 0][0], grid[fx + 1][fy + 0][1] });
	farm_grid.push_back({ grid[fx + 1][fy - 2][0], grid[fx + 1][fy - 2][1] });

	farm_grid.push_back({ grid[fx][fy + 7][0], grid[fx][fy + 7][1] });
	farm_grid.push_back({ grid[fx][fy + 5][0], grid[fx][fy + 5][1] });
	farm_grid.push_back({ grid[fx][fy + 3][0], grid[fx][fy + 3][1] });
	farm_grid.push_back({ grid[fx][fy + 1][0], grid[fx][fy + 1][1] });
	farm_grid.push_back({ grid[fx][fy - 1][0], grid[fx][fy - 1][1] });
	farm_grid.push_back({ grid[fx][fy - 3][0], grid[fx][fy - 3][1] });*/
	//////////////////////////////////////////////////////////
	//farm_grid.push_back({ grid[fx][fy + 8][0], grid[fx][fy + 8][1] });
	farm_grid.push_back({ grid[fx][fy + 8][0], grid[fx][fy + 8][1] + 12.5 });
	/*farm_grid.push_back({ grid[fx][fy + 6][0], grid[fx][fy + 6][1] });
	farm_grid.push_back({ grid[fx][fy + 4][0], grid[fx][fy + 4][1] });
	farm_grid.push_back({ grid[fx][fy + 2][0], grid[fx][fy + 2][1] });
	farm_grid.push_back({ grid[fx][fy + 0][0], grid[fx][fy + 0][1] });
	farm_grid.push_back({ grid[fx][fy - 2][0], grid[fx][fy - 2][1] });*/
	//farm_grid.push_back({ grid[fx][fy - 4][0], grid[fx][fy - 4][1] });
	farm_grid.push_back({ grid[fx][fy - 4][0], grid[fx][fy - 4][1] - 12.5 });

	///*farm_grid.push_back({ grid[fx - 1][fy + 7][0], grid[fx - 1][fy + 7][1] });
	//farm_grid.push_back({ grid[fx - 1][fy + 5][0], grid[fx - 1][fy + 5][1] });
	//farm_grid.push_back({ grid[fx - 1][fy + 3][0], grid[fx - 1][fy + 3][1] });
	//farm_grid.push_back({ grid[fx - 1][fy + 1][0], grid[fx - 1][fy + 1][1] });
	//farm_grid.push_back({ grid[fx - 1][fy - 1][0], grid[fx - 1][fy - 1][1] });
	//farm_grid.push_back({ grid[fx - 1][fy - 3][0], grid[fx - 1][fy - 3][1] });

	//farm_grid.push_back({ grid[fx - 1][fy + 6][0], grid[fx - 1][fy + 6][1] });
	//farm_grid.push_back({ grid[fx - 1][fy + 4][0], grid[fx - 1][fy + 4][1] });
	//farm_grid.push_back({ grid[fx - 1][fy + 2][0], grid[fx - 1][fy + 2][1] });
	//farm_grid.push_back({ grid[fx - 1][fy + 0][0], grid[fx - 1][fy + 0][1] });
	//farm_grid.push_back({ grid[fx - 1][fy - 2][0], grid[fx - 1][fy - 2][1] });

	//farm_grid.push_back({ grid[fx - 2][fy + 5][0], grid[fx - 2][fy + 5][1] });
	//farm_grid.push_back({ grid[fx - 2][fy + 3][0], grid[fx - 2][fy + 3][1] });
	//farm_grid.push_back({ grid[fx - 2][fy + 1][0], grid[fx - 2][fy + 1][1] });
	//farm_grid.push_back({ grid[fx - 2][fy - 1][0], grid[fx - 2][fy - 1][1] });

	//farm_grid.push_back({ grid[fx - 2][fy + 4][0], grid[fx - 2][fy + 4][1] });
	//farm_grid.push_back({ grid[fx - 2][fy + 2][0], grid[fx - 2][fy + 2][1] });
	//farm_grid.push_back({ grid[fx - 2][fy + 0][0], grid[fx - 2][fy + 0][1] });

	//farm_grid.push_back({ grid[fx - 3][fy + 3][0], grid[fx - 3][fy + 3][1] });
	//farm_grid.push_back({ grid[fx - 3][fy + 1][0], grid[fx - 3][fy + 1][1] });*/

	//farm_grid.push_back({ grid[fx - 3][fy + 2][0], grid[fx - 3][fy + 2][1] });
	farm_grid.push_back({ grid[fx - 3][fy + 2][0] - 17.5, grid[fx - 3][fy + 2][1] });

	int farm_mx, farm_my;

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

				for (int i = 0; i < grid_height; ++i)
				{
					for (int j = 0; j < grid_width; ++j)
					{
						grid[j][i][0] = (grid[j][i][0] - event.wheel.mouseX) * new_scale / scale + event.wheel.mouseX;
						grid[j][i][1] = (grid[j][i][1] - event.wheel.mouseY) * new_scale / scale + event.wheel.mouseY;
					}
				}

				for (auto& [x, y] : farm_grid)
				{
					x = (x - event.wheel.mouseX) * new_scale / scale + event.wheel.mouseX;
					y = (y - event.wheel.mouseY) * new_scale / scale + event.wheel.mouseY;
				}

				scale = new_scale;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				mouse_press_time = SDL_GetTicks();

				mouse_down = true;
				drag_start_x = event.button.x;
				drag_start_y = event.button.y;

				farm_mx = event.button.x;
				farm_my = event.button.y;
			}
			if (event.type == SDL_MOUSEBUTTONUP && mouse_down)
			{
				mouse_down = false;
				display_farm_grid = -1;
			}
			if (event.type == SDL_MOUSEMOTION && mouse_down && display_farm_grid == -1)
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

				for (int i = 0; i < grid_height; ++i)
				{
					for (int j = 0; j < grid_width; ++j)
					{
						grid[j][i][0] += delta_x;
						grid[j][i][1] += delta_y;
					}
				}

				for (auto& [x, y] : farm_grid)
				{
					x += delta_x;
					y += delta_y;
				}

				drag_start_x = event.motion.x;
				drag_start_y = event.motion.y;
			}
			if (event.type == SDL_MOUSEMOTION && display_farm_grid != -1)
			{
				int delta_x = event.motion.x - farm_mx;
				int delta_y = event.motion.y - farm_my;
				for (auto& [x, y] : farm_grid)
				{
					x += delta_x;
					y += delta_y;
				}
				positions[0][0] += delta_x;
				positions[0][1] += delta_y;
				farm_mx = event.motion.x;
				farm_my = event.motion.y;
			}
		}

		// Render your frame here

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

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

		if (SDL_GetTicks() - mouse_press_time >= 800 && mouse_down && display_farm_grid == -1)
		{
			mouse_press_time = SDL_GetTicks();

			// If mouse has been pressed for 800 ms, then assume player is trying to select a building
			// Loop through all neighboring rhombuses in the grid to see if any of them are part of a building

			int mx, my;
			SDL_GetMouseState(&mx, &my);

			if (isPointInRhombus(mx, my, (farm_grid[0].first + farm_grid[3].first)/2, (farm_grid[1].second + farm_grid[2].second)/2, 35 * 7 * scale, 25 * 7 * scale))
			{
				display_farm_grid = 5;
			}

		}

		std::vector<std::pair<float, float>> fg(farm_grid.begin(), farm_grid.end());
		for (auto& [x, y] : fg)
		{
			int sx = 0, sy = 0;
			for (int i = 0; i < grid_height; ++i)
			{
				for (int j = 0; j < grid_width; ++j)
				{
					if (dist(grid[j][i][0], grid[j][i][1], x, y) <
						dist(grid[sx][sy][0], grid[sx][sy][1], x, y))
					{
						sx = j;
						sy = i;
					}
				}
			}
			x = grid[sx][sy][0];
			y = grid[sx][sy][1];
		}

		const std::vector<SDL_Vertex> verts = {
			{ SDL_FPoint{fg[0].first + 17.5f * scale, fg[0].second}, SDL_Color{0, 255, 0, 255}, SDL_FPoint{0},},
			{ SDL_FPoint{fg[1].first, fg[1].second+12.5f* scale }, SDL_Color{0, 255, 0, 255}, SDL_FPoint{0},},
			{ SDL_FPoint{fg[2].first, fg[2].second -12.5f* scale }, SDL_Color{0, 255, 0, 255}, SDL_FPoint{0},}
		};
		SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);

		const std::vector<SDL_Vertex> vert = {
			{ SDL_FPoint{fg[1].first, fg[1].second + 12.5f * scale}, SDL_Color{0, 255, 0, 255}, SDL_FPoint{0},},
			{ SDL_FPoint{fg[2].first, fg[2].second  - 12.5f* scale}, SDL_Color{0, 255, 0, 255}, SDL_FPoint{0},},
			{ SDL_FPoint{fg[3].first - 17.5f* scale, fg[3].second}, SDL_Color{0, 255, 0, 255}, SDL_FPoint{0},}
		};
		SDL_RenderGeometry(renderer, nullptr, vert.data(), vert.size(), nullptr, 0);

		float w = 200 * scale;
		float h = farmhouse_height / (farmhouse_width / 200) * scale;

		for (int i = 0; i < 2; ++i)
		{
			SDL_FRect dest_rect = {
				(verts[0].position.x + vert[2].position.x) / 2.0 - w / 2.0 ,//positions[i][0] - w / 2.0,
				(verts[1].position.y + verts[2].position.y) / 2.0 - h / 2.0 - 12.5f*scale,//positions[i][1] - h / 2.0,
				w, h
			};

			
			SDL_RenderCopyF(renderer, farmhouse, NULL, &dest_rect);
		}







		/*int xx, yy;
		SDL_GetMouseState(&xx, &yy);
		float mx = xx, my = yy;
		int sx = 0, sy = 0;
		for (int i = 0; i < grid_height; ++i)
		{
			for (int j = 0; j < grid_width; ++j)
			{
				if (dist(grid[j][i][0], grid[j][i][1], mx, my) <
					dist(grid[sx][sy][0], grid[sx][sy][1], mx, my))
				{
					sx = j;
					sy = i;
				}
			}
		}
		mx = grid[sx][sy][0];
		my = grid[sx][sy][1];
		w = 35 * scale;
		h = 25 * scale;
		const std::vector<SDL_Vertex> verts1 = {
			{ SDL_FPoint{mx,		 my - h / 2}, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, },
			{ SDL_FPoint{mx + w / 2, my		  }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, },
			{ SDL_FPoint{mx,	 	 my + h / 2 }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, }

		};
		SDL_RenderGeometry(renderer, nullptr, verts1.data(), verts1.size(), nullptr, 0);

		std::vector<SDL_Vertex> vert1 = {
			{ SDL_FPoint{mx - w / 2, my }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, },
			{ SDL_FPoint{mx,		 my - h / 2 }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, },
			{ SDL_FPoint{mx,		 my + h / 2 }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, }
		};
		SDL_RenderGeometry(renderer, nullptr, vert1.data(), vert1.size(), nullptr, 0);*/


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
