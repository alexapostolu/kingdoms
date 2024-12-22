#pragma once

#include "object.hpp"
#include "resource_animation.hpp"
#include "grid.hpp"

#include "SDL.h"
#include "SDL_FontCache.h"

#include <forward_list>
#include <array>
#include <string>
#include <tuple>

enum class ResourceBuildingType
{
	FARMHOUSE,
	LUMBERMILL,
};

class ResourceBuilding
	: private Object
{
public:
	/*
	 * @param renderer To create the farmhouse texture
	 * @param pos The inital position used to calculate the grid snap
	 *   position
	 */
	ResourceBuilding(
		ResourceBuildingType _type,
		SDL_Renderer* _renderer,
		SDL_FPoint const& pos,
		Grid const& grid,
		float _scale,
		FC_Font* font
	);

	void init_resource_timer();

public:
	void pan(float dx, float dy);

	/*
	 * If mouse is pressed over object, returns True. Otherwise, handles mouse
	 * not pressed over object and returns False.
	 */
	bool mouse_press(float mx, float my);

	/*
	 * Handles updates for when mouse is pressed over object and returns
	 * the amount of resources collected from the mouse press.
	 */
	int mouse_press_update(float scale);

	/*
	 * @param farmhouses For collision detection
	 */
	void mouse_drag(float mx, float my, std::forward_list<ResourceBuilding> const& farmhouses, float scale);
	
	bool mouse_release();

	/*
	 * Zoom based on mouse position and scale
	 */
	void mouse_wheel(int mouse_x, int mouse_y, float scale_ratio);

	void render(SDL_Renderer* renderer, float scale);

	void render_information(SDL_Renderer* renderer, float scale);

	void update();

	static Uint32 resource_callback(Uint32 interval, void* obj);

private:
	bool is_rhombus_in_rhombus(std::array<SDL_Vertex, 4> const& _vertices) const;

public:
	static ResourceBuilding* drag_ptr;

public:
	ResourceBuildingType type;

	SDL_Renderer* renderer;

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

	bool display_resource;
	float resource_amount;
	float resource_per_sec;
	float resource_capacity;

	bool show_information;
	SDL_FRect info_rect;

private:
	/*
	 * Resource icon and collection animation.
	 */
	ResourceAnimation resource_animation;
	bool animate_collection;

	/*
	 * Information card.
	 */
	float information_width;
	FC_Font* font;
	FC_Font* info_tab_font;
	enum class InfoTab {
		Info,
		Stats,
		COUNT
	} tab;

	enum class Animate {
		Closed,
		Opening,
		Opened,
		Closing
	} animate;
};