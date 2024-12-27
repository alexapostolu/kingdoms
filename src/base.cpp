#include "base.hpp"

#include <iostream>


Base::Base(SDL_Window* _window, SDL_Renderer* _renderer, int sw, int sh)
	: Scene(_window, _renderer)
	, screen_width(sw), screen_height(sh)
	, wheat(0), wood(0)
	, drag_building(nullptr)
	, grid(40)
	, display_shop(false), shop_bar({ 0, screen_height - 250, screen_width, 300 })
	, font(FC_CreateFont())
	, new_building(false)
	, build_open({ sw * 0.85f, sh * 0.9f, sw * 0.1f, sh * 0.1f })
	, build_close({ screen_width * 0.9f, screen_height * 0.6f, screen_width * 0.1f, screen_height * 0.1f })
	, battle({ screen_width * 0.1f, screen_height * 0.9f, screen_width * 0.1f, screen_height * 0.1f })
{
	FC_LoadFont(font, renderer, "../../assets/Cinzel.ttf", screen_height / 24, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	shop_buildings = {
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, SDL_FPoint{ 120, (float)shop_bar.y + 100 }, grid, scale, font),
		ResourceBuilding(ResourceBuildingType::LUMBERMILL, renderer, SDL_FPoint{ 400, (float)shop_bar.y + 100 }, grid, scale, font)
	};

	resource_buildings = {
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, { 300, 200 }, grid, scale, font),
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, { 600, 300 }, grid, scale, font),
		ResourceBuilding(ResourceBuildingType::LUMBERMILL, renderer, { 900, 300 }, grid, scale, font)

	};
	for (auto& resource_building : resource_buildings)
		resource_building.init_resource_timer();
}


void Base::handle_scroll(int scroll) 
{
	static float const MIN_SCALE = 0.5f;
	static float const MAX_SCALE = 6.f;

	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);

	float new_scale = scale + scroll * 0.1f;
	if (new_scale >= MIN_SCALE && new_scale <= MAX_SCALE)
	{
		float scale_ratio = new_scale / scale;
		grid.mouse_wheel(mouse_x, mouse_y, scale_ratio);

		for (auto& resource_building : resource_buildings)
		{
			resource_building.mouse_wheel(mouse_x, mouse_y, scale_ratio);
		}

		scale = new_scale;
	}
}

void Base::handle_mouse_click(int mouse_x, int mouse_y)
{
	auto building = is_resource_building_clicked(mouse_x, mouse_y);
	if (building)
		building->mouse_press_update(scale, wheat, wood);

	if (!display_shop && build_open.is_clicked(mouse_x, mouse_y))
		display_shop = true;
	else if (display_shop && build_close.is_clicked(mouse_x, mouse_y))
		display_shop = false;

	if (!display_shop && battle.is_clicked(mouse_x, mouse_y))
		Scene::current_scene = Scene::scenes[1];
}

void Base::handle_mouse_drag_start(int mouse_x, int mouse_y)
{
	if (display_shop && (drag_building = is_shop_building_clicked(mouse_x, mouse_y)))
	{
		ResourceBuilding new_resource_building(
			drag_building->type,
			renderer, SDL_FPoint{ (float)mouse_x, (float)mouse_y }, grid, scale,
			font
		);

		resource_buildings.push_front(new_resource_building);
		resource_buildings.front().init_resource_timer();

		drag_building = &resource_buildings.front();

		new_building = true;

		drag = Drag::Building;
	}
	else if (drag_building = is_resource_building_clicked(mouse_x, mouse_y))
	{
		drag = Drag::Building;
	}
	else
	{
		drag = Drag::Grid;
	}

	// [Feature] The shop bar should automatically close when the grid or a
	// building is dragged.
	display_shop = false;

	drag_curr_x = mouse_x;
	drag_curr_y = mouse_y;
}

void Base::handle_mouse_drag_duration(int mouse_x, int mouse_y)
{
	float delta_x = mouse_x - drag_curr_x;
	float delta_y = mouse_y - drag_curr_y;

	if (drag == Drag::Building)
	{
		drag_building->mouse_drag(delta_x, delta_y, resource_buildings, scale);
	}
	else if (drag == Drag::Grid)
	{
		if (end_drag_x + delta_x < -screen_width / 2)
			stop_drag = 1;
		else if (end_drag_x + delta_x > screen_width / 2)
			stop_drag = -1;
		else if (end_drag_y + delta_y < -screen_height / 2)
			stop_drag = 2;
		else if (end_drag_y + delta_y > screen_height / 2)
			stop_drag = -2;

		end_drag_x += delta_x;
		end_drag_y += delta_y;

		grid.mouse_drag(delta_x, delta_y);

		for (auto& resource_building : resource_buildings)
			resource_building.pan(delta_x, delta_y);
	}

	drag_curr_x = mouse_x;
	drag_curr_y = mouse_y;
}

void Base::handle_mouse_drag_end(int mouse_x, int mouse_y)
{
	if (drag_building)
	{
		bool is_blocked = drag_building->mouse_release();
		if (is_blocked && new_building)
			resource_buildings.pop_front();

		drag_building = nullptr;
		new_building = false;
	}
}


void Base::update()
{
	update_buildings();
}

void Base::render()
{
	render_background();

	grid.render(renderer, scale);

	render_buildings();

	render_resource_bar();

	render_shop();
}


ResourceBuilding* Base::is_resource_building_clicked(int mouse_x, int mouse_y)
{
	for (auto& building : resource_buildings)
	{
		if (building.mouse_press(mouse_x, mouse_y))
			return &building;
	}

	return nullptr;
}

ResourceBuilding* Base::is_shop_building_clicked(int mouse_x, int mouse_y)
{
	for (auto& building : shop_buildings)
	{
		if (building.mouse_press(mouse_x, mouse_y))
			return &building;
	}

	return nullptr;
}

void Base::update_buildings()
{
	for (auto & resource_building : resource_buildings)
		resource_building.update();
}

void Base::render_background()
{
	SDL_SetRenderDrawColor(renderer, 82, 166, 84, 255);
	SDL_RenderClear(renderer);
}

void Base::render_buildings()
{
	for (auto& resource_building : resource_buildings)
		resource_building.render(renderer, scale);
}

void Base::render_resource_bar()
{
	SDL_Rect resource_bar{ 0, 0, screen_width, screen_height / 12 };
	SDL_RenderFillRect(renderer, &resource_bar);
	FC_SetDefaultColor(font, SDL_Color{ 255, 255, 255, 255 });
	FC_Draw(font, renderer, 0, screen_height / 24 - screen_height / 48, "Wheat: %d", wheat);
	FC_Draw(font, renderer, 300, screen_height / 24 - screen_height / 48, "Wood: %d", wood);
}

void Base::render_shop()
{
	if (!display_shop)
	{
		FC_SetDefaultColor(font, SDL_Color{ 0, 0, 0, 255 });

		FC_Draw(font, renderer, build_open.rect.x, build_open.rect.y, "Build");

		FC_Draw(font, renderer, battle.rect.x, battle.rect.y, "Battle");
	}
	else
	{
		FC_DrawColor(font, renderer, build_close.rect.x, build_close.rect.y, SDL_Color{ 0, 0, 0, 255 }, "Close");

		SDL_RenderFillRect(renderer, &shop_bar);

		for (auto& shop_resource_building : shop_buildings)
			shop_resource_building.render(renderer, 1);
	}
}