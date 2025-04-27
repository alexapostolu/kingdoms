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
	LUMBERMILL
};

/*
 * Out of bounds check when building the dragged is done by snapping the actual
 * location of the building (based off of the mouse position) to the nearest
 * grid tile. So the actual position of the building could be way out of bounds,
 * but it is always snapped to the nearest grid position.
 */
class ResourceBuilding : private Object
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
		FC_Font* font,
		int tiles_x, int tiles_y
	);

	void init_resource_timer();

public:
	void pan(float dx, float dy);

	/*
	 * If mouse is pressed over object, returns True. Otherwise, handles mouse
	 * not pressed over object and returns False.
	 */
	bool mouse_press(float mx, float my, float scale);

	/*
	 * Handles updates for when mouse is pressed over object and returns
	 * the amount of resources collected from the mouse press.
	 */
	void mouse_press_update(float scale, unsigned int& wheat, unsigned int& wood);

	/*
	 * @param farmhouses For collision detection
	 * 
	 * Only call this function if the object is being dragged. This function does not check if
	 * the mouse is dragging it, it just assumes it.
	 */
	void mouse_drag(float mx, float my, float scale);
	
	bool mouse_release(float scale);

	/*
	 * Zoom based on mouse position and scale
	 */
	void mouse_wheel(int mouse_x, int mouse_y, float scale_ratio);

	void render(SDL_Renderer* renderer, float scale);

	void render_information(SDL_Renderer* renderer, float scale);

	void update();

	static Uint32 resource_callback(Uint32 interval, void* obj);

public:
	int get_tiles_x() const;
	int get_tiles_y() const;

	float get_width(float scale) const;
	float get_height(float scale) const;

	float get_pos_x() const;
	float get_pos_y() const;

	void set_tiling_colour(SDL_Color const& colour);

private:
	void update_grid_pos(float scale);

	bool is_clicked(float mx, float my, float scale);

public:
	ResourceBuildingType type;

	Grid const& grid;

	SDL_Renderer* renderer;

	// Display
	SDL_Texture* texture;
	int texture_width, texture_height;

	// Position
	float actual_pos_x, actual_pos_y;
	float grid_pos_x, grid_pos_y;

	float offset_x, offset_y;

	bool display_resource;
	float resource_amount;
	float resource_per_sec;
	float resource_capacity;

	bool show_information;
	SDL_FRect info_rect;

private:
	SDL_Color tiling_colour;

	float starting_grid_pos_x, starting_grid_pos_y;

	int tiles_x, tiles_y;

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