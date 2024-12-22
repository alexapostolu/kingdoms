#include "resource_animation.hpp"

#include "resource_building.hpp"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_FontCache.h"

#include <math.h>

ResourceAnimation::ResourceAnimation(
	ResourceBuildingType type,
	SDL_Renderer* renderer
)
	: font(FC_CreateFont())
	, texture(nullptr), texture_w(0), texture_h(0)
	, start_animation(false)
{
	int screen_width, screen_height;
	SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

	char const* resource_img = nullptr;
	if (type == ResourceBuildingType::FARMHOUSE)
		resource_img = "../../assets/wheat.png";
	else if (type == ResourceBuildingType::LUMBERMILL)
		resource_img = "../../assets/wood.png";

	SDL_Surface* collect_icon_surface = IMG_Load(resource_img);

	texture_w = collect_icon_surface->w;
	texture_h = collect_icon_surface->h;

	texture = SDL_CreateTextureFromSurface(renderer, collect_icon_surface);

	SDL_FreeSurface(collect_icon_surface);

	// Font to display the number of resource collected.
	int font_size = screen_height / 15;
	FC_LoadFont(font, renderer, "../../assets/Cinzel.ttf", font_size, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
}

void ResourceAnimation::render_icon(SDL_Renderer* renderer, SDL_FRect& building_rect, float scale)
{
	SDL_FRect resource_rect{
		building_rect.x,
		building_rect.y,
		125 * scale,
		texture_h / (texture_w / 125) * scale
	};

	SDL_SetTextureAlphaMod(texture, 255);
	SDL_RenderCopyF(renderer, texture, NULL, &resource_rect);
}

bool ResourceAnimation::render_animation(SDL_Renderer* renderer, float scale)
{
	int font_x = font_rect.x + (font_rect.w / 2);
	int font_y = font_rect.y + (font_rect.h / 2);
	SDL_SetTextureAlphaMod(texture, (unsigned char)(fmin(255, transparency + 50)));
	SDL_RenderCopyF(renderer, texture, NULL, &font_rect);
	FC_SetDefaultColor(font, SDL_Color{ 255, 255, 255, (unsigned char)(fmin(255, transparency + 50)) });
	FC_DrawAlign(font, renderer, font_x + 50, font_y, FC_ALIGN_LEFT, "+ %d", (int)collected);
	FC_SetDefaultColor(font, { 255, 255, 255, 255 });
	font_rect.y -= 3;

	for (int i = 0; i < 7; ++i)
	{
		SDL_SetTextureAlphaMod(texture, transparency);
		SDL_RenderCopyF(renderer, texture, NULL, &std::get<0>(resources[i]));

		std::get<0>(resources[i]).x += std::get<1>(resources[i]);
		std::get<0>(resources[i]).y -= std::get<2>(resources[i]);

		std::get<2>(resources[i]) -= 0.5;

	}

	transparency -= 7;
	return transparency > 0;
}

void ResourceAnimation::init_animation(SDL_Renderer* renderer, float building_x, float building_y, float scale, float amount_collected)
{
	float x = building_x;
	float y = building_y - 75 * scale;
	float w = 120 * scale;
	float h = texture_h / (texture_w / 120) * scale;

	SDL_FRect dest_rect{
		x - (w / 2.0f), y - (h / 2.0f),
		w * 0.65f, h * 0.65f
	};

	font_rect = dest_rect;
	font_rect.w /= 0.65;
	font_rect.h /= 0.65;

	dest_rect.y += 20;

	for (int i = 0; i < 7; ++i)
	{
		int vel_x = (rand() % 8)  - 4;
		int vel_y = (rand() % 10) + 8;
		resources[i] = std::make_tuple(dest_rect, vel_x, vel_y);
	}

	start_animation = true;
	transparency = 255;
	collected = amount_collected;
}