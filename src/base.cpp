#include "base.hpp"

#include <iostream>


Base::Base()
	: wheat(0), wood(0)
	, drag(Drag::NONE)
	, grid(40, 60)
	//, font(FC_CreateFont())
{
	scale = 1;
	//FC_LoadFont(font, renderer, "../../assets/Cinzel.ttf", screen_height / 24, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
}


void Base::handle_scroll() 
{
}

void Base::handle_mouse_click(int mouse_x, int mouse_y)
{
	auto building = is_resource_building_clicked(mouse_x, mouse_y);
	
	if (building && building->type == ResourceBuildingType::FARMHOUSE)
		wheat += building->mouse_press_update(scale);
	if (building && building->type == ResourceBuildingType::LUMBERMILL)
		wood += building->mouse_press_update(scale);
}

void Base::handle_mouse_drag_start(int mouse_x, int mouse_y)
{
	if (is_resource_building_clicked(mouse_x, mouse_y))
		drag = Drag::Building;
	else
		drag = Drag::Grid;

	drag_curr_x = mouse_x;
	drag_curr_y = mouse_y;
}

void Base::handle_mouse_drag_duration(int mouse_x, int mouse_y)
{
	if (drag == Drag::Building)
	{
		float delta_x = mouse_x - drag_curr_x;
		float delta_y = mouse_y - drag_curr_y;

		ResourceBuilding::drag_ptr->mouse_drag(delta_x, delta_y, resource_buildings, scale);

		drag_curr_x = mouse_x;
		drag_curr_y = mouse_y;
	}
	else
	{
		if (display_shop)
		{
			for (auto& shop_resource_building : shop_buildings)
			{
				if (shop_resource_building.mouse_press(mouse_x, mouse_y))
				{
					// Create new resource building
					ResourceBuilding new_resource_building(
						shop_resource_building.type,
						renderer, SDL_FPoint{ (float)mouse_x, (float)mouse_y }, grid, scale,
						font);

					// Add it to resource buildings
					resource_buildings.push_front(new_resource_building);
					resource_buildings.front().init_resource_timer();
					ResourceBuilding::drag_ptr = &resource_buildings.front();

					display_shop = false;
					break;
				}
			}
		}
		else
		{
			float delta_x = mouse_x - drag_curr_x;
			float delta_y = mouse_y - drag_curr_y;

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

			drag_curr_x = mouse_x;
			drag_curr_y = mouse_y;
		}
	}
}

void Base::handle_mouse_drag_end(int mouse_x, int mouse_y)
{
}


void Base::update()
{
	update_buildings();
}

void Base::render()
{
	render_resource_bar();

	render_buildings();
}

void Base::switch_scene(scene_id next_id)
{

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

void Base::update_buildings()
{
	for (auto & resource_building : resource_buildings)
		resource_building.update();
}

void Base::render_resource_bar()
{
	SDL_Rect resource_bar{ 0, 0, screen_width, screen_height / 12 };
	SDL_RenderFillRect(renderer, &resource_bar);
	FC_SetDefaultColor(font, SDL_Color{ 255, 255, 255, 255 });
	FC_Draw(font, renderer, 0, screen_height / 24 - screen_height / 48, "Wheat: %d", wheat);
	FC_Draw(font, renderer, 300, screen_height / 24 - screen_height / 48, "Wood: %d", wood);
}

void Base::render_buildings()
{
	for (auto& resource_building : resource_buildings)
		resource_building.render(renderer, scale);
}