#pragma once

#include "scene.hpp"
#include "resource_building.hpp"

#include "SDL.h"

#include <forward_list>

class Base : public Scene
{
public:
	Base();

public:
	void handle_scroll();

	void handle_mouse_click(int mouse_x, int mouse_y);

	void handle_mouse_drag_start(int mouse_x, int mouse_y);

	void handle_mouse_drag_duration(int mouse_x, int mouse_y);

	void handle_mouse_drag_end(int mouse_x, int mouse_y);

	void update();

	void render();

	void switch_scene(scene_id next_id);

	SDL_Renderer* renderer;
	int screen_width;
	int screen_height;
	FC_Font* font;
	std::forward_list<ResourceBuilding> resource_buildings;
	std::forward_list<ResourceBuilding> shop_buildings;
	Grid grid;
	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;
	int stop_drag = 0;

private:
	/*
	 * @returns pointer of the resource building that the mouse has clicked.
	 * @returns nullptr if no resource building is clicked.
	 */
	ResourceBuilding* is_resource_building_clicked(int mouse_x, int mouse_y);

	/*
	 */
	void update_buildings();

	/*
	 * Renders the top bar displaying amount of resources.
	 */
	void render_resource_bar();

	/*
	 * Renders all the buildings and their associated animations.
	 */
	void render_buildings();

private:
	unsigned int wheat;
	unsigned int wood;

	enum class Drag {
		Building,
		Grid,
		NONE
	} drag;

	int drag_curr_x;
	int drag_curr_y;

	bool display_shop;
};