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
	, TILES_X(200), TILES_Y(200)
	, tiles(TILES_Y, std::vector<Tile>(TILES_X, Tile{ TileState::GRASS }))
{
	Screen::get().image_store("farmer.png");

	farmers.push_back({ TILES_X / 2.f, TILES_Y / 2.f });
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
	float spd = 0.2f;

	for (auto& farmer : farmers)
	{
		Screen::get().image_display("farmer.png", (int)(farmer.x * 2) + 100, (int)(farmer.y * 2) + 100, 100, 60);

		if (farmer.path.empty())
			farmer.generate_path(tiles);

		auto const& dest = farmer.path[0];
		float const epslion = 0.01f;

		if (std::fabs(farmer.x - dest.x) < epslion && std::fabs(farmer.y - dest.y) < epslion)
			farmer.path.pop_front();
		else if (farmer.x - dest.x > epslion)
			farmer.x -= spd;
		else if (dest.x - farmer.x > epslion)
			farmer.x += spd;
		else if (farmer.y - dest.y > epslion)
			farmer.y -= spd;
		else if (dest.y - farmer.y > epslion)
			farmer.y += spd;
	}
}

void Base::display_shop()
{
	static int shop_y = Screen::get().SCREEN_HEIGHT;
	const int shop_h = 300;
	const int shop_spd = 15;

	switch (shop_state)
	{
	case ShopState::HIDDEN: {
		Screen::get().text("BUILD", sdl2::clr_white, sdl2::str_brygada, 45,
			Screen::get().SCREEN_WIDTH - 20, Screen::get().SCREEN_HEIGHT - 65, sdl2::TTF_Align::RIGHT);

		break;
	}
	case ShopState::APPEARING: {
		Screen::get().rect(0, shop_y, Screen::get().SCREEN_WIDTH, shop_h, sdl2::clr_black);

		shop_y -= shop_spd;
		if (shop_y <= Screen::get().SCREEN_HEIGHT - shop_h)
		{
			shop_state = ShopState::VISIBLE;
			shop_y = Screen::get().SCREEN_HEIGHT - shop_h;
		}

		break;
	}
	case ShopState::VISIBLE: {
		Screen::get().rect(0, Screen::get().SCREEN_HEIGHT - shop_h, Screen::get().SCREEN_WIDTH, shop_h, sdl2::clr_black);

		Screen::get().text("CLOSE", sdl2::clr_white, sdl2::str_brygada, 35,
			Screen::get().SCREEN_WIDTH - 15, Screen::get().SCREEN_HEIGHT - shop_h - 40, sdl2::TTF_Align::RIGHT);

		break;
	}
	case ShopState::DISAPPEARING: {
		Screen::get().rect(0, shop_y, Screen::get().SCREEN_WIDTH, shop_h, sdl2::clr_black);

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
	if (shop_state == ShopState::HIDDEN && x >= 1010 && x <= 1145 && y >= 470 && y <= 500)
		shop_state = ShopState::APPEARING;
	else if (shop_state == ShopState::VISIBLE && x >= 1055 && x <= 1165 && y >= 190 && y <= 220)
		shop_state = ShopState::DISAPPEARING;
}