#include "grid.hpp"

#include "SDL.h"

const float rhombus_w = 35.0f;
const float rhombus_h = 25.0f;

Grid::Grid(int _side_length, int screen_width_mid, int screen_height_mid)
	: side_length(_side_length)
	, data(nullptr)
{
	float starting_x = screen_width_mid;
	float starting_y = screen_height_mid - (side_length * (rhombus_h / 2));

	data = new SDL_FPoint*[side_length];
	for (int i = 0; i < side_length; ++i)
	{
		data[i] = new SDL_FPoint[side_length];
		for (int j = 0; j < side_length; ++j)
		{
			float x = starting_x + ((rhombus_w / 2) * j);
			float y = starting_y + ((rhombus_h / 2) * j);

			data[i][j] = { x, y };
		}

		starting_x -= rhombus_w / 2;
		starting_y += rhombus_h / 2;
	}
}

Grid::~Grid()
{
	for (int i = 0; i < side_length; ++i)
		delete[] data[i];

	delete[] data;
}

void Grid::mouse_wheel(int mouse_x, int mouse_y, float scale_ratio)
{
	for (int i = 0; i < side_length; ++i)
	{
		for (int j = 0; j < side_length; ++j)
		{
			data[i][j].x = (data[i][j].x - mouse_x) * scale_ratio + mouse_x;
			data[i][j].y = (data[i][j].y - mouse_y) * scale_ratio + mouse_y;
		}
	}
}

void Grid::mouse_drag(float dx, float dy)
{
	for (int i = 0; i < side_length; ++i)
	{
		for (int j = 0; j < side_length; ++j)
			pan_point(data[i][j], dx, dy);
	}
}

void Grid::render(SDL_Renderer* renderer, float scale)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	for (int i = 0; i < side_length; ++i)
	{
		for (int j = 0; j < side_length; ++j)
		{
			float x = data[i][j].x;
			float y = data[i][j].y;
			float w = rhombus_w * scale;
			float h = rhombus_h * scale;
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