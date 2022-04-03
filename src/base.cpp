#include "base.hpp"
#include "screen.hpp"
#include "person.hpp"
#include "tile.hpp"
#include <building.hpp>
#include "sdl2.hpp"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <vector>
#include <map>

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
	, text_build(Screen::get().SCREEN_WIDTH - 20, Screen::get().SCREEN_HEIGHT - 65, sdl2::Align::RIGHT)
{
	//farmers.push_back(Person{ { TILES_X / 2, TILES_Y / 2 }, { TILES_X / 2.f * 20 + 5, TILES_Y / 2.f * 20 + 60 } });

	shop_buildings.push_back({ "farmhouse.png",	 { 200, Screen::get().SCREEN_HEIGHT - 150 + 20, 300, 170 }, 100, 0, 0 });
	shop_buildings.push_back({ "lumbermill.png", { 450, Screen::get().SCREEN_HEIGHT - 150, 300, 170 },		100, 0, 0 });
	shop_buildings.push_back({ "road.png",		 { 650, Screen::get().SCREEN_HEIGHT - 150, 80, 40 },		10, 0, 0 });
}

void Base::display_resources()
{
	Screen::get().rect(0, 0, Screen::get().SCREEN_WIDTH, 50, sdl2::clr_black, std::nullopt);
	Screen::get().rect(0, 50, Screen::get().SCREEN_WIDTH, 5, sdl2::clr_yellow, std::nullopt);

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
		pos[0] + margin, 10, sdl2::Align::LEFT);
	
	Screen::get().text(wheat, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[1] + margin, 10, sdl2::Align::LEFT);

	Screen::get().text(wood, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[2] + margin, 10, sdl2::Align::LEFT);

	Screen::get().text(gems, sdl2::clr_yellow, sdl2::str_brygada, 24,
		pos[3] + margin, 10, sdl2::Align::LEFT);
}

void Base::display_scene()
{
	static int tick = 0;

	float spd = 0.5f;
	static int step_size = 20 / spd;

	for (auto& farmer : farmers)
	{
		Screen::get().image("farmer.png",
			(int)farmer.actual_pos.x, (int)farmer.actual_pos.y, 100, 60, sdl2::Align::CENTER);

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

	for (std::size_t i = 0; i < base_buildings.size(); ++i)
	{
		if (i == place)
			continue;

		auto const& [img, dim, cost_g, cost_w, cost_s] = base_buildings[i];
		Screen::get().image(img, dim.x, dim.y, dim.w, dim.h, sdl2::Align::CENTER);

		if (img == "lumbermill.png" && tick > 500)
		{
			gold++;
			wood++;
			tick = 0;
		}
		tick++;
	}

	if (place != -1)
	{
		auto& [img, dim, cost_g, cost_w, cost_s] = base_buildings[place];

		int x, y;
		SDL_GetMouseState(&x, &y);

		dim.x = ((x - 5) / 20) * 20;
		dim.y = (y / 20) * 20;

		Screen::get().rect(dim.x, dim.y, dim.w, dim.h, sdl2::clr_black, std::nullopt);
		Screen::get().image(img, dim.x, dim.y, dim.w, dim.h, sdl2::Align::CENTER);

		int const img_mid = (dim.x + (dim.x + dim.w)) / 2;
		Screen::get().image("checkmark.png", img_mid - 60, dim.y + dim.h + 10, 40, 40, sdl2::Align::CENTER);
		Screen::get().image("x.png",		 img_mid + 60, dim.y + dim.h + 10, 40, 40, sdl2::Align::CENTER);
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
		if (place == -1)
		{
			Screen::get().text("BUILD", sdl2::clr_white, sdl2::str_brygada, 45,
				text_build.dim.x, text_build.dim.y, text_build.align);

			shop_y = Screen::get().SCREEN_HEIGHT;
		}
		else
		{
			for (int i = 0; i < TILES_Y; ++i)
			{
				for (int j = 0; j < TILES_X; ++j)
					Screen::get().rect((j * 20) + 5, (i * 20) + 60, 20, 20, std::nullopt, sdl2::clr_white);
			}
		}

		break;
	}
	case ShopState::APPEARING: {
		Screen::get().rect(0, shop_y, Screen::get().SCREEN_WIDTH, Screen::get().SCREEN_HEIGHT - shop_h, sdl2::clr_black, std::nullopt);

		shop_y -= shop_spd;
		if (shop_y <= shop_h)
		{
			shop_state = ShopState::VISIBLE;
			shop_y = shop_h;
		}

		break;
	}
	case ShopState::VISIBLE: {
		Screen::get().rect(0, shop_h, Screen::get().SCREEN_WIDTH, Screen::get().SCREEN_HEIGHT - shop_h, sdl2::clr_black, std::nullopt);

		Screen::get().text("CLOSE", sdl2::clr_white, sdl2::str_brygada, 35,
			Screen::get().SCREEN_WIDTH - 15, shop_h - 40, sdl2::Align::RIGHT);

		for (auto const& building : shop_buildings)
		{
			auto const& [img, dim, cost_g, cost_w, cost_s] = building;
			Screen::get().image(img, dim.x, dim.y, dim.w, dim.h, sdl2::Align::CENTER);
		}

		break;
	}
	case ShopState::DISAPPEARING: {
		Screen::get().rect(0, shop_y, Screen::get().SCREEN_WIDTH, Screen::get().SCREEN_HEIGHT - shop_h, sdl2::clr_black, std::nullopt);

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
		if (text_build.clicked_on(x, y))
		{
			place = -1;
			shop_state = ShopState::APPEARING;
		}
		else if (place != -1)
		{
			auto const& [img, dim, cost_g, cost_w, cost_s] = base_buildings[place];
			
			if (img == "road.png")
				tiles[(y - 60) / 20][(x - 60) / 20].state = TileState::PATH;

			bool can_place = true;
			for (int i = (dim.x - 60) / 20; i < (dim.x + dim.w - 60) / 20; ++i)
			{
				for (int j = (dim.y - 60) / 20; j < (dim.y + dim.h - 60) / 20; ++j)
					can_place = tiles[j][i].state != TileState::OCCUPIED;
			}

			if (can_place)
			{
				for (int i = (dim.x - 60) / 20; i < (dim.x + dim.w - 60) / 20; ++i)
				{
					for (int j = (dim.y - 60) / 20; j < (dim.y + dim.h - 60) / 20; ++j)
						tiles[j][i].state = TileState::OCCUPIED;
				}

				gold -= cost_g;
				wood -= cost_w;
				stone -= cost_s;
				place = -1;
			}
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
			auto const& [img, dim, cost_g, cost_w, cost_s] = building;
			if (x >= dim.x && x <= dim.x + dim.w &&
				y >= dim.y && y <= dim.y + dim.h)
			{
				base_buildings.push_back({ img, { dim.x, dim.y, (int)(dim.w / 1.5), (int)(dim.h / 1.4) }, cost_g, cost_w, cost_s });
				place = base_buildings.size() - 1;

				shop_state = ShopState::HIDDEN;
				return;
			}
		}
	}
}