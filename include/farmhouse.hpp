#pragma once

#include "object.hpp"
#include "grid.hpp"

#include "SDL.h"

#include <forward_list>
#include <array>
#include <optional>

namespace king {

class Farmhouse : private Object
{
public:
	/*
	 * @param renderer To create the farmhouse texture
	 * @param pos The inital position used to calculate the grid snap
	 *   position
	 */
	Farmhouse(
		SDL_Renderer* renderer,
		SDL_FPoint const& pos,
		Grid const& grid,
		float _scale
	);

	void init_resource_timer();

	~Farmhouse();

public:
	void pan(float dx, float dy);

	/*
	 * @returns True if mouse is pressed over the object
	 */
	bool mouse_press(float mx, float my);
	int mouse_press_update();

	/*
	 * @param farmhouses For collision detection
	 */
	void mouse_drag(float mx, float my, std::forward_list<Farmhouse> const& farmhouses, float scale);
	
	void mouse_release();

	/*
	 * Zoom based on mouse position and scale
	 */
	void mouse_wheel(int mouse_x, int mouse_y, float scale_ratio);

	void render(SDL_Renderer* renderer, float scale);

	void update();

	static Uint32 resource_callback(Uint32 interval, void* obj);

private:
	bool is_rhombus_in_rhombus(std::array<SDL_Vertex, 4> const& _vertices) const;

public:
	inline static Farmhouse* drag_ptr = nullptr;

public:
	// Display
	SDL_Texture* texture;
	int texture_width, texture_height;
	SDL_Colour clr;

	// Position
	float offset_x, offset_y;
	std::array<SDL_Vertex, 4> grid_snap_vertices;
	std::array<SDL_Vertex, 4> absolute_vertices;

	// Movement
	float start_mouse_drag_x, start_mouse_drag_y;
	// If the movement ends at an invalid location, reset the position to original
	bool record_start_vertices;
	std::array<SDL_Vertex, 4> start_grid_snap_vertices;
	std::array<SDL_Vertex, 4> start_absolute_vertices;

	// Resourse Generation
	SDL_Texture* resource_texture;
	int resource_texture_width, resource_texture_height;
	bool display_resource;
	float resource_amount;
	float resource_per_sec;
};

} // namespace king