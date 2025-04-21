#pragma once

#include "scene.hpp"
#include "resource_building.hpp"
#include "button.hpp"

#include "SDL.h"

#include <forward_list>

class Base : public Scene
{
public:
	Base(SDL_Window* _window, SDL_Renderer* _renderer, int sw, int sh);

public:
	void handle_scroll(int scroll) override;

	void handle_mouse_click(int mouse_x, int mouse_y) override;

	void handle_mouse_drag_start(int mouse_x, int mouse_y) override;

	void handle_mouse_drag_duration(int mouse_x, int mouse_y) override;

	void handle_mouse_drag_end(int mouse_x, int mouse_y) override;

	void update() override;

	void render() override;

private:
	/*
	 * @returns pointer of the resource building that the mouse has clicked.
	 * @returns nullptr if no resource building is clicked.
	 */
	ResourceBuilding* is_resource_building_clicked(int mouse_x, int mouse_y);

	/*
	 * 
	 */
	ResourceBuilding* is_shop_building_clicked(int mouse_x, int mouse_y);

	/*
	 * 
	 */
	void update_buildings();

	/*
	 * 
	 */
	void render_background();

	/*
	 * Renders all the buildings and their associated animations.
	 */
	void render_buildings();

	/*
	 * Renders the top bar displaying amount of resources.
	 */
	void render_resource_bar();

	/*
	 * 
	 */
	void render_shop();

private:
	// [REMOVE]
	int screen_width, screen_height;

	Grid grid;

	enum class Drag {
		Building,
		Grid
	} drag;

	int drag_curr_x;
	int drag_curr_y;

	ResourceBuilding* drag_building;
	bool new_building;

	bool display_shop;
	SDL_Rect shop_bar;

	std::forward_list<ResourceBuilding> resource_buildings;
	std::forward_list<ResourceBuilding> shop_buildings;

	int drag_start_x;
	int drag_start_y;
	int end_drag_x = 0, end_drag_y = 0;
	int stop_drag = 0;

	Button build_open;
	Button build_close;

	Button battle;

	FC_Font* font;

	unsigned int wheat;
	unsigned int wood;
};