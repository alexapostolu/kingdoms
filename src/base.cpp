#include "base.hpp"
#include "screen.hpp"
#include "person.hpp"
#include "tile.hpp"
#include "sdl2.hpp"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <vector>

Base& Base::get()
{
	static Base instance;
	return instance;
}

Base::Base()
	: gold(750), wheat(250), wood(500), stone(0), gems(10)
	, level(1), exp(0), troph(0)
	, edit_mode(false), shop_state(ShopState::HIDDEN)
	, TILES_X(58), TILES_Y(23)
	, tiles(TILES_Y, std::vector<Tile>(TILES_X, Tile{ TileState::GRASS }))
	, place(-1)
{
	Screen::get().image_store("farmer.png");
	Screen::get().image_store("farmhouse.png");
	Screen::get().image_store("lumbermill.png");

	farmers.push_back(Person{ { TILES_X / 2, TILES_Y / 2 }, { TILES_X / 2.f * 20 + 5, TILES_Y / 2.f * 20 + 60 } });

	shop_buildings.push_back({ "farmhouse.png", { 20, (Screen::get().SCREEN_HEIGHT - 300) + 20, 300, 170 } });
	shop_buildings.push_back({ "lumbermill.png", { 350, (Screen::get().SCREEN_HEIGHT - 300) + 20, 300, 170 } });
}

void Base::display_resources()
{
	Screen::get().rect(0, 0, Screen::get().SCREEN_WIDTH, 50, sdl2::clr_black);
	Screen::get().rect(0, 50, Screen::get().SCREEN_WIDTH, 5, sdl2::clr_yellow);

	std::string gold = "Gold: " + std::to_string(Base::get().gold);
	std::string wheat = "Wheat: " + std::to_string(Base::get().wheat);
	std::string wood = "Wood: " + std::to_string(Base::get().wood);
	std::string gems = "Gems: " + std::to_string(Base::get().gems);

	int pos[] = {
		0,
		(Screen::get().SCREEN_WIDTH / 4),
		(Screen::get().SCREEN_WIDTH / 4 * 2),
		(Screen::get().SCREEN_WIDTH / 4 * 3)
	};


	sdl2::font_ptr ttf_font(TTF_OpenFont(sdl2::str_brygada.c_str(), 24));
	int text_w, text_h;
	int s = TTF_SizeText(ttf_font.get(), gems.c_str(), &text_w, &text_h);
	int margin = (Screen::get().SCREEN_WIDTH - (Screen::get().SCREEN_WIDTH / 4 * 3 + text_w)) / 2;

	Screen::get().text(gold, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[0] + margin, 10, sdl2::TTF_Align::LEFT);
	
	Screen::get().text(wheat, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[1] + margin, 10, sdl2::TTF_Align::LEFT);

	Screen::get().text(wood, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[2] + margin, 10, sdl2::TTF_Align::LEFT);

	Screen::get().text(gems, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[3] + margin, 10, sdl2::TTF_Align::LEFT);
}

void Base::display_scene()
{
	float spd = 0.5f;
	static int step_size = 20 / spd;

	for (auto& farmer : farmers)
	{
		Screen::get().image_display("farmer.png",
			(int)farmer.actual_pos.x, (int)farmer.actual_pos.y, 100, 60);

		if (farmer.path.empty())
			farmer.generate_path(tiles);

		auto const& dest = farmer.path[0];
		if (step_size == 0)
		{
			farmer.path_pos = farmer.path[0];
			farmer.path.pop_front();

			step_size = 20 / spd;
		}
		else
		{
			if (farmer.path_pos.x < dest.x)
				farmer.actual_pos.x += spd;
			else if (farmer.path_pos.x > dest.x)
				farmer.actual_pos.x -= spd;
			else if (farmer.path_pos.y < dest.y)
				farmer.actual_pos.y += spd;
			else if (farmer.path_pos.y > dest.y)
				farmer.actual_pos.y -= spd;

			step_size--;
		}
	}

	// show grid
	/*
	for (int i = 0; i < tiles.size(); ++i)
	{
		for (int j = 0; j < tiles[i].size(); ++j)
		{
			Screen::get().rect(j * 20 + 5, i * 20 + 60, 20, 20, sdl2::clr_black);
		}
	}
	*/

	for (std::size_t i = 0; i < base_buildings.size(); ++i)
	{
		if (i == place)
			continue;

		auto const& [img, pos] = base_buildings[i];
		Screen::get().image_display(img, pos[0], pos[1], pos[2], pos[3]);
	}

	if (place != -1)
	{
		auto& [img, pos] = base_buildings[place];
		int x, y;
		SDL_GetMouseState(&x, &y);
		pos[0] = x;
		pos[1] = y;
		Screen::get().image_display(img, pos[0], pos[1], pos[2], pos[3]);
	}
}

void Base::display_shop()
{
	static int shop_y = Screen::get().SCREEN_HEIGHT;
	const int shop_h = Screen::get().SCREEN_HEIGHT - 300;
	const int shop_spd = 15;

	switch (shop_state)
	{
	case ShopState::HIDDEN: {
		Screen::get().text("BUILD", sdl2::clr_white, sdl2::str_brygada, 45,
			Screen::get().SCREEN_WIDTH - 20, Screen::get().SCREEN_HEIGHT - 65, sdl2::TTF_Align::RIGHT);

		shop_y = Screen::get().SCREEN_HEIGHT;
		break;
	}
	case ShopState::APPEARING: {
		Screen::get().rect(0, shop_y, Screen::get().SCREEN_WIDTH, Screen::get().SCREEN_HEIGHT - shop_h, sdl2::clr_black);

		shop_y -= shop_spd;
		if (shop_y <= shop_h)
		{
			shop_state = ShopState::VISIBLE;
			shop_y = shop_h;
		}

		break;
	}
	case ShopState::VISIBLE: {
		Screen::get().rect(0, shop_h, Screen::get().SCREEN_WIDTH, Screen::get().SCREEN_HEIGHT - shop_h, sdl2::clr_black);

		Screen::get().text("CLOSE", sdl2::clr_white, sdl2::str_brygada, 35,
			Screen::get().SCREEN_WIDTH - 15, shop_h - 40, sdl2::TTF_Align::RIGHT);

		for (auto const& building : shop_buildings)
		{
			auto const& [img, pos] = building;
			Screen::get().image_display(img, pos[0], pos[1], pos[2], pos[3]);
		}

		break;
	}
	case ShopState::DISAPPEARING: {
		Screen::get().rect(0, shop_y, Screen::get().SCREEN_WIDTH, Screen::get().SCREEN_HEIGHT - shop_h, sdl2::clr_black);

		shop_y += shop_spd;
		if (shop_y >= Screen::get().SCREEN_HEIGHT)
		{
			shop_state = ShopState::HIDDEN;
			shop_y = Screen::get().SCREEN_HEIGHT;
		}

		break;
	}
	}
}

void Base::handle_mouse_on_shop(int x, int y)
{
	if (shop_state == ShopState::HIDDEN)
	{
		if (x >= 1010 && x <= 1145 && y >= 470 && y <= 500)
		{
			place = -1;
			shop_state = ShopState::APPEARING;
		}
		else if (place != -1)
		{
			place = -1;
		}
	}
	else if (shop_state == ShopState::VISIBLE)
	{
		if (x >= 1055 && x <= 1165 && y >= 190 && y <= 220)
		{
			shop_state = ShopState::DISAPPEARING;
			return;
		}
		
		for (auto const& building : shop_buildings)
		{
			auto const& [img, pos] = building;
			if (x >= pos[0] && x <= pos[0] + pos[2] &&
				y >= pos[1] && y <= pos[1] + pos[3])
			{
				base_buildings.push_back({ img, { pos[0], pos[1], (int)(pos[2] / 1.5), (int)(pos[3] / 1.5) } });
				place = base_buildings.size() - 1;

				shop_state = ShopState::HIDDEN;
				return;
			}
		}
	}
}