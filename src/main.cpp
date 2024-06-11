#include "SDL.h"
#include "SDL_image.h"

#include <Windows.h> // GetSystemMetrics
#include <iostream>
#include <vector>

bool is_point_in_polygon(std::vector<std::pair< float, float>> const& vertices, float px, float py)
{
	bool collision = false;

	// go through each of the vertices, plus
	// the next vertex in the list
	int next = 0;
	for (int current = 0; current < vertices.size(); current++)
	{

		// get next vertex in list
		// if we've hit the end, wrap around to 0
		next = current + 1;
		if (next == vertices.size()) next = 0;

		// get the PVectors at our current position
		// this makes our if statement a little cleaner
		std::pair< float, float> const& vc = vertices[current];    // c for "current"
		std::pair< float, float> const& vn = vertices[next];       // n for "next"

		// compare position, flip 'collision' variable
		// back and forth
		if (((vc.second >= py && vn.second < py) || (vc.second < py && vn.second >= py)) &&
			(px < (vn.first - vc.first) * (py - vc.second) / (vn.second - vc.second) + vc.first)) {
			collision = !collision;
		}
	}
	return collision;
}

// Function to determine the orientation of the triplet (p, q, r)
int orientation(std::pair<float, float> const& p, std::pair<float, float> const& q, std::pair<float, float> const& r) {
    float val = (q.second - p.second) * (r.first - q.first) - (q.first - p.first) * (r.second - q.second);
    if (val == 0.0) return 0; // collinear
    return (val > 0.0) ? 1 : 2; // clock or counterclock wise
}

// Function to check if line segments p1q1 and p2q2 intersect
bool do_lines_intersect(std::pair<float, float> const& p1, std::pair<float, float> const& q1, std::pair<float, float> const& p2, std::pair<float, float> const& q2) {
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4) return true;

    // Special cases
	auto on_segment = [](std::pair<float, float> const& p, std::pair<float, float> const& q, std::pair<float, float> const& r) {
		return q.first <= fmax(p.first, r.first) && q.first >= fmin(p.first, r.first) &&
			q.second <= fmax(p.second, r.second) && q.second >= fmin(p.second, r.second);
	};

    if (o1 == 0 && on_segment(p1, p2, q1)) return true;
    if (o2 == 0 && on_segment(p1, q2, q1)) return true;
    if (o3 == 0 && on_segment(p2, p1, q2)) return true;
    if (o4 == 0 && on_segment(p2, q1, q2)) return true;

    return false;
}

// Function to check for collision detection between two rhombuses
bool is_rhombus_in_rhombus(std::vector<std::pair<float, float>> const& rhombus1, std::vector<std::pair<float, float>> const& rhombus2)
{
    // Check for vertex containment
    for (int i = 0; i < 4; ++i)
	{
        if (is_point_in_polygon(rhombus2, rhombus1[i].first, rhombus1[i].second) ||
			is_point_in_polygon(rhombus1, rhombus2[i].first, rhombus2[i].second))
			return true;
    }

    return false;
}

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
	float positions[2][2] = { { 305, 200 }, { 605, 300} };
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
	for (int i = 0; i < grid_height; ++i)
	{
		for (int j = 0; j < grid_width; ++j)
		{
			grid[j][i][0] = j * 35 + (i % 2 == 0 ? 0 : 17.5);
			grid[j][i][1] = i * 12.5;
		}
	}
	Uint32 mouse_press_time = 0;

	int display_farm_grid = -1;

	std::vector<std::pair<float, float>> farm_grid1;
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

	farm_grid1.push_back({ grid[fx + 2][fy + 3][0] + 17.5, grid[fx + 2][fy + 3][1] });
	farm_grid1.push_back({ grid[fx][fy + 8][0], grid[fx][fy + 8][1] + 12.5 });
	farm_grid1.push_back({ grid[fx][fy - 2][0], grid[fx][fy - 2][1] - 12.5 });
	farm_grid1.push_back({ grid[fx - 3][fy + 3][0] - 17.5, grid[fx - 3][fy + 3][1] });
	/*farm_grid.push_back({grid[fx + 3][fy + 2][0], grid[fx + 3][fy + 2][1]});
	farm_grid.push_back({ grid[fx + 2][fy + 3][0], grid[fx + 2][fy + 3][1] });
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
	farm_grid.push_back({ grid[fx][fy - 3][0], grid[fx][fy - 3][1] });
	farm_grid.push_back({ grid[fx][fy + 8][0], grid[fx][fy + 8][1] });
	farm_grid.push_back({ grid[fx][fy + 6][0], grid[fx][fy + 6][1] });
	farm_grid.push_back({ grid[fx][fy + 4][0], grid[fx][fy + 4][1] });
	farm_grid.push_back({ grid[fx][fy + 2][0], grid[fx][fy + 2][1] });
	farm_grid.push_back({ grid[fx][fy + 0][0], grid[fx][fy + 0][1] });
	farm_grid.push_back({ grid[fx][fy - 2][0], grid[fx][fy - 2][1] });
	farm_grid.push_back({ grid[fx][fy - 4][0], grid[fx][fy - 4][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 7][0], grid[fx - 1][fy + 7][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 5][0], grid[fx - 1][fy + 5][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 3][0], grid[fx - 1][fy + 3][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 1][0], grid[fx - 1][fy + 1][1] });
	farm_grid.push_back({ grid[fx - 1][fy - 1][0], grid[fx - 1][fy - 1][1] });
	farm_grid.push_back({ grid[fx - 1][fy - 3][0], grid[fx - 1][fy - 3][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 6][0], grid[fx - 1][fy + 6][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 4][0], grid[fx - 1][fy + 4][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 2][0], grid[fx - 1][fy + 2][1] });
	farm_grid.push_back({ grid[fx - 1][fy + 0][0], grid[fx - 1][fy + 0][1] });
	farm_grid.push_back({ grid[fx - 1][fy - 2][0], grid[fx - 1][fy - 2][1] });
	farm_grid.push_back({ grid[fx - 2][fy + 5][0], grid[fx - 2][fy + 5][1] });
	farm_grid.push_back({ grid[fx - 2][fy + 3][0], grid[fx - 2][fy + 3][1] });
	farm_grid.push_back({ grid[fx - 2][fy + 1][0], grid[fx - 2][fy + 1][1] });
	farm_grid.push_back({ grid[fx - 2][fy - 1][0], grid[fx - 2][fy - 1][1] });
	farm_grid.push_back({ grid[fx - 2][fy + 4][0], grid[fx - 2][fy + 4][1] });
	farm_grid.push_back({ grid[fx - 2][fy + 2][0], grid[fx - 2][fy + 2][1] });
	farm_grid.push_back({ grid[fx - 2][fy + 0][0], grid[fx - 2][fy + 0][1] });
	farm_grid.push_back({ grid[fx - 3][fy + 3][0], grid[fx - 3][fy + 3][1] });
	farm_grid.push_back({ grid[fx - 3][fy + 1][0], grid[fx - 3][fy + 1][1] });
	farm_grid.push_back({ grid[fx - 3][fy + 2][0], grid[fx - 3][fy + 2][1] });*/

	std::vector<std::pair<float, float>> farm_grid2;
	// Find closest rhombus to center of farm grid. This rhombus is the center.
	int fx2 = 0, fy2 = 0;
	for (int i = 0; i < grid_height; ++i)
	{
		for (int j = 0; j < grid_width; ++j)
		{
			if (dist(grid[j][i][0], grid[j][i][1], positions[1][0], positions[1][1]) <
				dist(grid[fx2][fy2][0], grid[fx2][fy2][1], positions[1][0], positions[1][1]))
			{
				fx2 = j;
				fy2 = i;
			}
		}
	}

	farm_grid2.push_back({ grid[fx2 + 2][fy2 + 3][0] + 17.5, grid[fx2 + 2][fy2 + 3][1] });
	farm_grid2.push_back({ grid[fx2][fy2 - 2][0], grid[fx2][fy2 - 2][1] - 12.5 });
	farm_grid2.push_back({ grid[fx2][fy2 + 8][0], grid[fx2][fy2 + 8][1] + 12.5 });
	farm_grid2.push_back({ grid[fx2 - 3][fy2 + 3][0] - 17.5, grid[fx2 - 3][fy2 + 3][1] });

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

				// Prevent scrolling too close or far
				if (new_scale < 0.5f || new_scale > 6.f)
					break;

				// Handle position offset from scroll
				for (int i = 0; i < 2; ++i)
				{
					// Find offset from position to mouse
					// Multiply that by new scale to get new offset
					// Divide by old scale to get relative offset?? - that what gpt4o said and it worked
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

				for (auto& [x, y] : farm_grid1)
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

				for (auto& [x, y] : farm_grid1)
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
				for (auto& [x, y] : farm_grid1)
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

		if (SDL_GetTicks() - mouse_press_time >= 800 && mouse_down && display_farm_grid == -1)
		{
			mouse_press_time = SDL_GetTicks();

			// If mouse has been pressed for 800 ms, then assume player is trying to select a building
			// Loop through all neighboring rhombuses in the grid to see if any of them are part of a building

			int mx, my;
			SDL_GetMouseState(&mx, &my);

			if (isPointInRhombus(mx, my, (farm_grid1[0].first + farm_grid1[3].first)/2, (farm_grid1[1].second + farm_grid1[2].second)/2, 35 * 7 * scale, 25 * 7 * scale))
			{
				display_farm_grid = 1;
			}

		}

		std::vector<std::pair<float, float>> fg(farm_grid1.begin(), farm_grid1.end());
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

		std::vector<std::pair<float, float>> fg2(farm_grid2.begin(), farm_grid2.end());
		for (auto& [x, y] : fg2)
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

	
		auto clr = is_rhombus_in_rhombus({
			std::make_pair(fg[0].first + 17.5f * scale, fg[0].second), std::make_pair(fg[1].first, fg[1].second + 12.5f * scale),
			std::make_pair(fg[2].first, fg[2].second - 12.5f * scale), std::make_pair(fg[3].first - 17.5f * scale, fg[3].second) },
			{
			std::make_pair(fg2[0].first + 17.5f * scale, fg2[0].second), std::make_pair(fg2[1].first, fg2[1].second + 12.5f * scale),
			std::make_pair(fg2[2].first, fg2[2].second - 12.5f * scale), std::make_pair(fg2[3].first - 17.5f * scale, fg2[3].second) })
			? SDL_Colour{255, 0, 0, 255} : SDL_Colour{ 0, 255, 0, 255 };

		const std::vector<SDL_Vertex> verts = {
			{ SDL_FPoint{fg[0].first + 17.5f * scale, fg[0].second}, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg[1].first, fg[1].second+12.5f* scale }, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg[2].first, fg[2].second -12.5f* scale },clr, SDL_FPoint{0},}
		};
		SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);

		const std::vector<SDL_Vertex> vert = {
			{ SDL_FPoint{fg[1].first, fg[1].second + 12.5f * scale}, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg[2].first, fg[2].second  - 12.5f* scale}, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg[3].first - 17.5f* scale, fg[3].second},  clr, SDL_FPoint{0},}
		};
		const std::vector<SDL_Vertex> vert2 = {
			{ SDL_FPoint{fg2[1].first, fg2[1].second - 12.5f * scale}, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg2[2].first, fg2[2].second + 12.5f * scale}, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg2[3].first - 17.5f * scale, fg2[3].second}, clr, SDL_FPoint{0},}
		};
		const std::vector<SDL_Vertex> verts2 = {
			{ SDL_FPoint{fg2[0].first + 17.5f * scale, fg2[0].second},  clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg2[1].first, fg2[1].second - 12.5f * scale }, clr, SDL_FPoint{0},},
			{ SDL_FPoint{fg2[2].first, fg2[2].second + 12.5f * scale }, clr, SDL_FPoint{0},}
		};
		SDL_RenderGeometry(renderer, nullptr, vert.data(), vert.size(), nullptr, 0);

		float w = 200 * scale;
		float h = farmhouse_height / (farmhouse_width / 200) * scale;
		SDL_FRect dest_rect = {
				(verts[0].position.x + vert[2].position.x) / 2.0 - w / 2.0,
				(verts[1].position.y + verts[2].position.y) / 2.0 - h / 2.0 - 12.5f * scale - (20 * scale),
				w, h
		};
		SDL_RenderCopyF(renderer, farmhouse, NULL, &dest_rect);


		SDL_RenderGeometry(renderer, nullptr, verts2.data(), verts2.size(), nullptr, 0);

		SDL_RenderGeometry(renderer, nullptr, vert2.data(), vert2.size(), nullptr, 0);

		SDL_FRect dest_rect2 = {
				(verts2[0].position.x + vert2[2].position.x) / 2.0 - w / 2.0,
				(verts2[1].position.y + verts2[2].position.y) / 2.0 - h / 2.0 - 12.5f * scale - (20 * scale),
				w, h
		};
		SDL_RenderCopyF(renderer, farmhouse, NULL, &dest_rect2);

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
