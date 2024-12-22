#include "grid.hpp"

#include "SDL.h"

Grid::Grid(int _width, int _height)
	: width(_width), height(_height), data(nullptr)
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

Grid::~Grid()
{
	for (int y = 0; y < height; ++y)
		delete[] data[y];

	delete[] data;
}

void Grid::mouse_wheel(int mouse_x, int mouse_y, float scale_ratio)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			data[i][j].x = (data[i][j].x - mouse_x) * scale_ratio + mouse_x;
			data[i][j].y = (data[i][j].y - mouse_y) * scale_ratio + mouse_y;
		}
	}
}

void Grid::mouse_drag(float dx, float dy)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			pan_point(data[i][j], dx, dy);
		}
	}
}

void Grid::render(SDL_Renderer* renderer, float scale)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			float x = data[i][j].x;
			float y = data[i][j].y;
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
}