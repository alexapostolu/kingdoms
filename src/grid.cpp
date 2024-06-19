#include "grid.hpp"

#include "SDL.h"

king::Grid::Grid(int _width, int _height)
	: width(_width), height(_height), old_scale(1)
{
	data = new SDL_FPoint*[height];
	for (int i = 0; i < height; ++i)
	{
		data[i] = new SDL_FPoint[width];
		for (int j = 0; j < width; ++j)
		{
			data[i][j] = SDL_FPoint();
			data[i][j].x = j * 35 + (i % 2 == 0 ? 0 : 17.5f);
			data[i][j].y = i * 12.5f;
		}
	}
}

king::Grid::~Grid()
{
	for (int y = 0; y < height; ++y)
		delete[] data[y];

	delete[] data;
}

void king::Grid::mouse_wheel(SDL_MouseWheelEvent const& wheel)
{
	int scroll = wheel.y;
	float new_scale = old_scale + scroll * 0.1f;

	// Prevent scrolling too close or far
	if (new_scale < 0.5f || new_scale > 6.f)
		return;

	// Find offset from position to mouse
	// Multiply that by new scale to get new offset
	// Divide by old scale to get relative offset?? - that what gpt4o said and it worked
	// Update the new position to be from the offset of mouse

	int mouse_x = wheel.mouseX;
	int mouse_y = wheel.mouseY;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			data[i][j].x = (data[i][j].x - mouse_x) * new_scale / old_scale + mouse_x;
			data[i][j].y = (data[i][j].y - mouse_y) * new_scale / old_scale + mouse_y;
		}
	}

	old_scale = new_scale;
}

void king::Grid::mouse_drag(float dx, float dy)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			pan_point(data[i][j], dx, dy);
		}
	}
}

void king::Grid::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			float x = data[i][j].x;
			float y = data[i][j].y;
			float w = 35 * old_scale;
			float h = 25 * old_scale;
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
}