/*
 * Resource class for handling and displaying resources for the resource
 * buildings.
 *
 * When a resource building has reached a certain storage threshold, it will
 * display the resource icon for the user to collect. When collected, a small
 * animation will play confirming the resources have been collected. This class
 * handles both the events for a resource building.
 */

#pragma once

#include "SDL.h"
#include "SDL_FontCache.h"

#include <array>
#include <tuple>

enum class ResourceBuildingType;

class ResourceAnimation
{
public:
	ResourceAnimation(
		ResourceBuildingType type,
		SDL_Renderer* renderer
	);

	void render_icon(SDL_Renderer* renderer, SDL_FRect& building_rect, float scale);

	bool render_animation(SDL_Renderer* renderer, float scale);

	/*
	 * Initializes texture positions and amount of resources collected for the
	 * animation.
	 */
	void init_animation(SDL_Renderer* renderer, float building_x, float building_y, float scale, float amount_collected);

private:
	// Texture displayed when a building reaches its storage threshold.
	SDL_Texture* texture;
	int texture_w, texture_h;

	// Font displaying the amount of resources collected.
	FC_Font* font;
	SDL_FRect font_rect;

	bool start_animation;
	float collected;
	std::array<std::tuple<SDL_FRect, float, float>, 7> resources;
	int transparency;
};