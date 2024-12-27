#include "battle.hpp"

#include "SDL_image.h"
#include "SDL_FontCache.h"


Battle::Battle(SDL_Window* _window, SDL_Renderer* _renderer, int sw, int sh)
	: Scene(_window, _renderer)
	, screen_width(sw), screen_height(sh)
	, font(FC_CreateFont())
	, end({ screen_width * 0.85f, screen_height * 0.9f, screen_width * 0.1f, screen_height * 0.1f })
	, start({ screen_width * 0.1f, screen_height * 0.9f, screen_width * 0.1f, screen_height * 0.1f })
{
	FC_LoadFont(font, renderer, "../../assets/Cinzel.ttf", screen_height / 24, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	SDL_Surface* surface = IMG_Load("../../assets/farmer.png");

	tw = surface->w;
	th = surface->h;

	t = SDL_CreateTextureFromSurface(renderer, surface);

	pos.push_back({ 0, 0, 150, th / (tw / 150) });

	SDL_FreeSurface(surface);
}


void Battle::handle_scroll(int scroll)
{

}

void Battle::handle_mouse_click(int mouse_x, int mouse_y)
{
	if (end.is_clicked(mouse_x, mouse_y))
		Scene::current_scene = Scene::scenes[0];
}

void Battle::handle_mouse_drag_start(int mouse_x, int mouse_y)
{

}

void Battle::handle_mouse_drag_duration(int mouse_x, int mouse_y)
{

}

void Battle::handle_mouse_drag_end(int mouse_x, int mouse_y)
{

}


void Battle::update()
{

}

void Battle::render()
{
	render_background();

	for (auto const& p : pos)
		SDL_RenderCopy(renderer, t, NULL, &p);
}


void Battle::render_background() const
{
	SDL_SetRenderDrawColor(renderer, 82, 166, 84, 255);
	SDL_RenderClear(renderer);

	FC_SetDefaultColor(font, SDL_Color{ 0, 0, 0, 255 });
	FC_Draw(font, renderer, end.rect.x, end.rect.y, "End");
	FC_Draw(font, renderer, start.rect.x, start.rect.y, "Start");
}