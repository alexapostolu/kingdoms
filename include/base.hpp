#pragma once

class Base
{
public:
    static Base& get();

public:
    Base(Base const&) = delete;
    void operator=(Base const&) = delete;

public:
    void display_resources();
    void display_shop();

    void handle_mouse_on_shop(int x, int y);

public:
	int gold, wheat, wood, stone, gems;
	int level, exp, troph;
    bool edit_mode;
    enum class ShopState
    {
        HIDDEN,
        VISIBLE,
        APPEARING,
        DISAPPEARING
    } shop_state;

private:
    Base();
};