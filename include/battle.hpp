#pragma once

#include "scene.hpp"
#include "button.hpp"

#include "SDL.h"
#include "SDL_FontCache.h"

#include <vector>

class Battle : public Scene
{
public:
	Battle(SDL_Window* _window, SDL_Renderer* _renderer, int sw, int sh);

public:
	void handle_scroll(int scroll) override;

	void handle_mouse_click(int mouse_x, int mouse_y) override;

	void handle_mouse_drag_start(int mouse_x, int mouse_y) override;

	void handle_mouse_drag_duration(int mouse_x, int mouse_y) override;

	void handle_mouse_drag_end(int mouse_x, int mouse_y) override;

	void update() override;

	void render() override;

private:
	void render_background() const;

private:
	int screen_width, screen_height;

	int tw, th;
	SDL_Texture* t;
	
	std::vector<SDL_Rect> pos;

	FC_Font* font;
	Button end;
	Button start;
};