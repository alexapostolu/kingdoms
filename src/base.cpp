#include "base.hpp"
#include "colours.hpp"
#include "math.hpp"

#include <iostream>
#include <cmath>

float lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

Base::Base(SDL_Window* _window, SDL_Renderer* _renderer, int sw, int sh)
	: Scene(_window, _renderer)
	, screen_width(sw), screen_height(sh)
	, grid(40, screen_width / 2, screen_height / 2)
	, x(screen_width / 2), y(screen_height / 2)
	, drag_building(nullptr)
	, drag(Drag::None)
	, display_shop(false), shop_bar({ 0, screen_height - 250, screen_width, 300 })
	, font(FC_CreateFont())
	, new_building(false)
	, build_open({ sw * 0.85f, sh * 0.9f, sw * 0.1f, sh * 0.1f })
	, build_close({ screen_width * 0.9f, screen_height * 0.6f, screen_width * 0.1f, screen_height * 0.1f })
	, battle({ screen_width * 0.1f, screen_height * 0.9f, screen_width * 0.1f, screen_height * 0.1f })
	, wheat(0), wood(0)
	, zoom_x(0), zoom_y(0)
{
	FC_LoadFont(font, renderer, "../../assets/Cinzel.ttf", screen_height / 24, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	shop_buildings = {
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, SDL_FPoint{ 120, (float)shop_bar.y + 100 }, grid, scale, font, 3, 3),
		ResourceBuilding(ResourceBuildingType::LUMBERMILL, renderer, SDL_FPoint{ 400, (float)shop_bar.y + 100 }, grid, scale, font, 4, 4)
	};

	resource_buildings = {
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, { 300, 200 }, grid, scale, font, 3, 3),
		ResourceBuilding(ResourceBuildingType::FARMHOUSE, renderer, { 600, 300 }, grid, scale, font, 3, 3),
		ResourceBuilding(ResourceBuildingType::LUMBERMILL, renderer, { 900, 300 }, grid, scale, font, 4, 4)

	};
	for (auto& resource_building : resource_buildings)
		resource_building.init_resource_timer();
}


void Base::handle_scroll(int scroll) 
{
	SDL_GetMouseState(&zoom_x, &zoom_y);

	float new_scale = scale + scroll * 0.1f;
	float scale_ratio = new_scale / scale;

	// [Feature] Prevent the user from zooming in infinitly small if they zoom
	// in faster than it can rebound back.
	if (new_scale < 0.5f)
		return;

	grid.mouse_wheel(zoom_x, zoom_y, scale_ratio);

	for (auto& resource_building : resource_buildings)
		resource_building.mouse_wheel(zoom_x, zoom_y, scale_ratio);

	scale = new_scale;
}

void Base::handle_mouse_click(int mouse_x, int mouse_y)
{
	auto building = is_resource_building_clicked(mouse_x, mouse_y, scale);
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
	if (display_shop && (drag_building = is_shop_building_clicked(mouse_x, mouse_y, scale)))
	{
		ResourceBuilding new_resource_building(
			drag_building->get_type(),
			renderer, SDL_FPoint{ (float)mouse_x, (float)mouse_y }, grid, scale,
			font, drag_building->get_tiles_x(), drag_building->get_tiles_y()
		);

		resource_buildings.push_front(new_resource_building);
		resource_buildings.front().init_resource_timer();

		drag_building = &resource_buildings.front();

		new_building = true;

		drag = Drag::Building;
	}
	else if (drag_building = is_resource_building_clicked(mouse_x, mouse_y, scale))
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

	x += delta_x;
	y += delta_y;

	if (drag == Drag::Building)
	{
		drag_building->mouse_drag(delta_x, delta_y, scale);

		resource_buildings.sort([](auto const& b1, auto const& b2) { return b1.get_pos_y() < b2.get_pos_y(); });

		for (auto const& building : resource_buildings)
		{
			if (&building != drag_building &&
				is_rhombus_in_rhombus(building.get_pos_x(), building.get_pos_y(), building.get_width(scale), building.get_height(scale),
					drag_building->get_pos_x(), drag_building->get_pos_y(), drag_building->get_width(scale), drag_building->get_height(scale)))
			{
				drag_building->set_tiling_colour(colour::building_tiling_red);
				break;
			}
		}
	}
	else if (drag == Drag::Grid)
	{
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
		bool is_blocked = drag_building->mouse_release(scale);
		if (is_blocked && new_building)
			resource_buildings.pop_front();

		drag_building = nullptr;
		new_building = false;
	}

	drag = Drag::None;
}


void Base::update()
{
	static float const PAN_X_MIN = 0;
	static float const PAN_X_MAX = screen_width;

	static float const PAN_Y_MIN = 0;
	static float const PAN_Y_MAX = screen_height;

	float const SCALE_MIN = 0.7f;
	float const SCALE_MAX = 6.0f;

	// 60 fps
	float t = 0.01f * 0.016f * 60.0f;

	update_buildings();

	if (drag == Drag::None)
	{
		float dx;
		float dy;

		if (x < PAN_X_MIN)
		{
			float mx = lerp(x, PAN_X_MIN, t);
			dx = mx - x;

			x = mx;

			if (x > PAN_X_MIN)
				x = PAN_X_MIN;
		}
		else if (x > PAN_X_MAX)
		{
			float mx = lerp(x, PAN_X_MAX, t);
			dx = mx - x;

			x = mx;

			if (x < PAN_X_MAX)
				x = PAN_X_MAX;
		}

		if (y < PAN_Y_MIN)
		{
			float my = lerp(y, PAN_Y_MIN, t);
			dy = my - y;

			y = my;

			if (y > PAN_Y_MIN)
				y = PAN_Y_MIN;
		}
		else if (y > PAN_Y_MAX)
		{
			float my = lerp(y, PAN_Y_MAX, t);
			dy = my - y;

			y = my;

			if (y < PAN_Y_MAX)
				y = PAN_Y_MAX;
		}

		grid.mouse_drag(dx, dy);
		for (auto& resource_building : resource_buildings)
			resource_building.pan(dx, dy);
	}

	float dscale = 0.0f;

	if (scale < SCALE_MIN)
	{
		float newScale = lerp(scale, SCALE_MIN, t);
		dscale = newScale - scale;
		scale = newScale;
		if (scale > SCALE_MIN) scale = SCALE_MIN;
	}
	else if (scale > SCALE_MAX)
	{
		float newScale = lerp(scale, SCALE_MAX, t);
		dscale = newScale - scale;
		scale = newScale;
		if (scale < SCALE_MAX) scale = SCALE_MAX;
	}

	float scale_ratio = scale / (scale - dscale);
	grid.mouse_wheel(zoom_x, zoom_y, scale_ratio);
	for (auto& resource_building : resource_buildings)
		resource_building.mouse_wheel(zoom_x, zoom_y, scale_ratio);
}

void Base::render()
{
	render_background();

	grid.render(renderer, scale);

	render_buildings();

	render_resource_bar();

	render_shop();
}


ResourceBuilding* Base::is_resource_building_clicked(int mouse_x, int mouse_y, float scale)
{
	for (auto& building : resource_buildings)
	{
		if (building.mouse_press(mouse_x, mouse_y, scale))
			return &building;
	}

	return nullptr;
}

ResourceBuilding* Base::is_shop_building_clicked(int mouse_x, int mouse_y, float scale)
{
	for (auto& building : shop_buildings)
	{
		if (building.mouse_press(mouse_x, mouse_y, scale))
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
	FC_Draw(font, renderer, 0, screen_height / 24.0f - screen_height / 48.0f, "Wheat: %d", wheat);
	FC_Draw(font, renderer, 300, screen_height / 24.0f - screen_height / 48.0f, "Wood: %d", wood);
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