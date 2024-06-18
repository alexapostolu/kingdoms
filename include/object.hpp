#pragma once

#include "SDL.h"

#include <array>
#include <vector>
#include <forward_list>

namespace king {

struct Object
{
protected:
	void pan_point(SDL_FPoint& point, float dx, float dy);
};

class Farmhouse : private Object
{
public:
	Farmhouse(SDL_Renderer* renderer, std::array<SDL_Vertex, 4> const& _grid_snap_vertices);
	~Farmhouse();

public:
	void pan(float dx, float dy);
	/*
	 * @returns True if mouse is pressed over the object
	 */
	bool mouse_pressed(float mx, float my);
	void drag(float mx, float my, std::forward_list<Farmhouse> const& farmhouses);
	void mouse_released();
	void zoom(SDL_MouseWheelEvent const& wheel);
	void render(SDL_Renderer* renderer);

	bool isPointInObject(int px, int py) const;
	bool isPolygonInObject(std::array<SDL_Vertex, 4> const& _vertices) const;

public:
	inline static Farmhouse* drag_ptr = nullptr;

public:
	// Display
	SDL_Texture* texture;
	int texture_width, texture_height;
	SDL_Colour clr;

	// Position
	std::array<SDL_Vertex, 4> grid_snap_vertices;
	std::array<SDL_Vertex, 4> absolute_vertices;

	// Movement
	float start_mouse_drag_x, start_mouse_drag_y;
	// If the movement ends at an invalid location, reset the position to original
	std::array<SDL_Vertex, 4> start_grid_snap_vertices;
	std::array<SDL_Vertex, 4> start_absolute_vertices;

	// Scaling
	float old_scale;
};

} // namespace king