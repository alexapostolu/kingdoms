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

public:
	int gold, wheat, wood, stone, gems;
	int level, exp, troph;

private:
    Base();

private:
    bool edit_mode, shop_visible;
};