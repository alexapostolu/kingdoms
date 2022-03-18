#include "base.hpp"
#include "screen.hpp"
#include "sdl2.hpp"

#include <iostream>
#include <string>

Base& Base::get()
{
	static Base instance;
	return instance;
}

Base::Base()
	: gold(750), wheat(250), wood(500), stone(0), gems(10)
	, level(1), exp(0), troph(0)
	, edit_mode(false), shop_state(ShopState::HIDDEN)

{
	
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

void Base::display_shop()
{
	static int shop_x = Screen::get().SCREEN_HEIGHT;
	const int shop_h = 100;

	switch (shop_state)
	{
	case ShopState::HIDDEN: {
		Screen::get().text("BUILD", sdl2::clr_white, sdl2::str_brygada, 45,
			20, Screen::get().SCREEN_HEIGHT - 65, sdl2::TTF_Align::RIGHT);
		break;
	}
	case ShopState::APPEARING: {
		Screen::get().text("BUILD", sdl2::clr_white, sdl2::str_brygada, 45,
			20, Screen::get().SCREEN_HEIGHT - 65, sdl2::TTF_Align::RIGHT);

		Screen::get().rect(0, shop_x, Screen::get().SCREEN_WIDTH, shop_h, sdl2::clr_black);

		shop_x -= 5;
		if (shop_x == Screen::get().SCREEN_HEIGHT - shop_h)
		{
			shop_state = ShopState::VISIBLE;
			shop_x = Screen::get().SCREEN_HEIGHT - shop_h;
		}

		break;
	}
	}
}

bool Base::mouse_on_shop(int x, int y)
{
	return shop_state == ShopState::HIDDEN &&
		x >= 1010 && x <= 1145 && y >= 470 && y <= 500;
}