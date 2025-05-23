#include "resource_building.hpp"
#include "colours.hpp"
#include "math.hpp"

#include <math.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_FontCache.h"

float dist(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void resource_building_type_to_image(ResourceBuildingType type, std::string& image, std::string& image_resource)
{
	switch (type)
	{
	case ResourceBuildingType::FARMHOUSE:
		image = "../../assets/farmhouse.png";
		image_resource = "../../assets/wheat.png";
		break;
	case ResourceBuildingType::LUMBERMILL:
		image = "../../assets/lumbermill.png";
		image_resource = "../../assets/wood.png";
		break;
	}
}

ResourceBuilding::ResourceBuilding(
	ResourceBuildingType _type,
	SDL_Renderer* _renderer,
	SDL_FPoint const& pos,
	Grid const& _grid,
	float _scale,
	FC_Font* _font,
	int _tiles_x, int _tiles_y
)
	: type(_type)
	, actual_pos_x(pos.x), actual_pos_y(pos.y)
	, grid_pos_x(0), grid_pos_y(0)
	, renderer(_renderer)
	, grid(_grid)
	, texture(nullptr), texture_width(-1), texture_height(-1), tiling_colour({ 255, 255, 255, 122 })
	, offset_x(-1), offset_y(-1)
	, display_resource(false)
	, resource_amount(0), resource_per_sec(0), resource_capacity(500)
	, show_information(false), info_rect()
	, animate(Animate::Closed), information_width(0)
	, font(_font)
	, resource_animation(_type, _renderer), animate_collection(false)
	, info_tab_font(FC_CreateFont())
	, tab(InfoTab::Info)
	, tiles_x(_tiles_x), tiles_y(_tiles_y)
	, state(PLACED)
{
	int screen_width, screen_height;
	SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

	std::string image;
	std::string resource_image;
	resource_building_type_to_image(_type, image, resource_image);

	SDL_Surface* surface = IMG_Load(image.c_str());
	if (!surface)
	{
		SDL_Log("Failed to load image: %s\n", IMG_GetError());
		return;
	}

	texture_width = surface->w;
	texture_height = surface->h;

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		SDL_Log("Failed to create texture: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(surface);

	offset_x = 0;
	offset_y = 25;

	update_grid_pos(_scale);

	info_rect.x = grid_pos_x;
	info_rect.y = grid_pos_y;
	info_rect.w = 80;
	info_rect.h = 80;

	int font_size = screen_height / 50;
	FC_LoadFont(info_tab_font, renderer, "../../assets/Cinzel.ttf", font_size, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	for (int i = 0; i < grid.side_length; ++i)
	{
		for (int j = 0; j < grid.side_length; ++j)
		{
			if (is_point_in_rhombus(grid_pos_x, grid_pos_y, get_width(_scale), get_height(_scale), grid.data[i][j].x, grid.data[i][j].y))
				grid.data[i][j].occupied = true;
		}
	}
}

void ResourceBuilding::init_resource_timer()
{
	SDL_AddTimer(3000, ResourceBuilding::resource_callback, this);
}

void ResourceBuilding::pan(float dx, float dy)
{
	actual_pos_x += dx;
	actual_pos_y += dy;

	grid_pos_x += dx;
	grid_pos_y += dy;
}

bool ResourceBuilding::is_clicked(float mx, float my, float scale)
{
	float width = 35 * scale * tiles_x;
	float height = 25 * scale * tiles_y;

	float dx = std::abs(mx - grid_pos_x);
	float dy = std::abs(my - grid_pos_y);

	float dist_norm = (dx / (width / 2.0)) + (dy / (height / 2.0));

	return dist_norm <= 1.0;
}

bool ResourceBuilding::mouse_press(float mx, float my, float scale)
{
	if (!is_clicked(mx, my, scale))
		return false;

	state = DRAGGING_OK;

	starting_grid_pos_x = grid_pos_x;
	starting_grid_pos_y = grid_pos_y;

	show_information = false;

	if (animate == Animate::Opened)
	{
		int screen_width, screen_height;
		SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

		// Close information card if mouse clicked away from the card.
		if (mx < screen_width - information_width)
		{
			animate = Animate::Closing;
		}
		else
		{
			for (int i = 0; i < (int)InfoTab::COUNT; ++i)
			{
				SDL_Rect icon_rect{
					(screen_width - information_width) + (i * 50) + 15, screen_height / 6,
					50, 50
				};

				// Mouse is in the icon.
				if (mx > icon_rect.x && mx < icon_rect.x + icon_rect.w &&
					my > icon_rect.y && my < icon_rect.y + icon_rect.h)
				{
					tab = (InfoTab)i;
					break;
				}
			}
		}
	}

	return true;
}

void ResourceBuilding::mouse_press_update(float scale, unsigned int& wheat, unsigned int& wood)
{
	if (display_resource)
	{
		//resource_animation.init_animation(renderer, grid_snap_vertices[0].position.x, grid_snap_vertices[1].position.y, scale, resource_amount);
		resource_animation.init_animation(renderer, grid_pos_x, grid_pos_y, scale, resource_amount);

		animate_collection = true;
		display_resource = false;

		if (type == ResourceBuildingType::FARMHOUSE)
			wheat += resource_amount;
		else if (type == ResourceBuildingType::LUMBERMILL)
			wood += resource_amount;

		resource_amount = 0;
	}
	else
	{
		if (animate == Animate::Closed)
			animate = Animate::Opening;
		else if (animate == Animate::Opened)
			animate = Animate::Closing;
	}
}

void ResourceBuilding::mouse_drag(float dx, float dy, float scale)
{
	actual_pos_x += dx;
	actual_pos_y += dy;

	update_grid_pos(scale);

	// Temporarily clear the building's previous position's occupied flags to avoid self-collision
	for (int i = 0; i < grid.side_length; ++i)
	{
		for (int j = 0; j < grid.side_length; ++j)
		{
			if (is_point_in_rhombus(starting_grid_pos_x, starting_grid_pos_y, get_width(scale), get_height(scale), grid.data[i][j].x, grid.data[i][j].y))
				grid.data[i][j].occupied = false;
		}
	}

	// Check for occupied tiles at the new position
	state = DRAGGING_OK;
	for (int i = 0; i < grid.side_length; ++i)
	{
		for (int j = 0; j < grid.side_length; ++j)
		{
			if (is_point_in_rhombus(grid_pos_x, grid_pos_y, get_width(scale), get_height(scale), grid.data[i][j].x, grid.data[i][j].y) &&
				grid.data[i][j].occupied)
			{
				state = DRAGGING_OCCUPIED;
				break;
			}
		}
		if (state == DRAGGING_OCCUPIED)
			break;
	}

	show_information = false;
}

bool ResourceBuilding::mouse_release(float scale)
{
	state = PLACED;

	// Check for occupied tiles at the new position
	bool collision = false;
	for (int i = 0; i < grid.side_length; ++i)
	{
		for (int j = 0; j < grid.side_length; ++j)
		{
			if (is_point_in_rhombus(grid_pos_x, grid_pos_y, get_width(scale), get_height(scale), grid.data[i][j].x, grid.data[i][j].y) &&
				grid.data[i][j].occupied)
			{
				collision = true;
				break;
			}
		}
		if (collision)
			break;
	}

	if (collision)
	{
		// Revert to starting position if collision detected
		grid_pos_x = starting_grid_pos_x;
		grid_pos_y = starting_grid_pos_y;
		actual_pos_x = starting_grid_pos_x;
		actual_pos_y = starting_grid_pos_y;
		return true;
	}
	else
	{
		// Move is valid: update actual_pos and grid occupancy
		actual_pos_x = grid_pos_x;
		actual_pos_y = grid_pos_y;

		// Clear previous position's occupied flags
		for (int i = 0; i < grid.side_length; ++i)
		{
			for (int j = 0; j < grid.side_length; ++j)
			{
				if (is_point_in_rhombus(starting_grid_pos_x, starting_grid_pos_y, get_width(scale), get_height(scale), grid.data[i][j].x, grid.data[i][j].y))
					grid.data[i][j].occupied = false;
			}
		}

		// Mark new position's tiles as occupied
		for (int i = 0; i < grid.side_length; ++i)
		{
			for (int j = 0; j < grid.side_length; ++j)
			{
				if (is_point_in_rhombus(grid_pos_x, grid_pos_y, get_width(scale), get_height(scale), grid.data[i][j].x, grid.data[i][j].y))
					grid.data[i][j].occupied = true;
			}
		}

		show_information = !show_information;
		return false;
	}
}

void ResourceBuilding::mouse_wheel(int mouse_x, int mouse_y, float scale_ratio)
{
	grid_pos_x = (grid_pos_x - mouse_x) * scale_ratio + mouse_x;
	grid_pos_y = (grid_pos_y - mouse_y) * scale_ratio + mouse_y;

	actual_pos_x = (actual_pos_x - mouse_x) * scale_ratio + mouse_x;
	actual_pos_y = (actual_pos_y - mouse_y) * scale_ratio + mouse_y;
}

void ResourceBuilding::render(SDL_Renderer* renderer, float scale)
{
	int width_half = (tiles_x * 35 * scale) / 2;
	int height_half = (tiles_y * 25 * scale) / 2;

	// Render green grid base
	int indices[] = {
		0, 1, 2, // Top, Right, Bottom
		0, 3, 2  // Top, Bottom, Left
	};

	// Define vertices with position, color, and texture coordinates
	if (state == DRAGGING_OCCUPIED)
		tiling_colour = colour::building_tiling_red;
	else if (state == DRAGGING_OK)
		tiling_colour = colour::building_tiling_green;
	else
		tiling_colour = SDL_Color{ 255, 255, 255, 122 };
	SDL_Vertex pos[] = {
		// Top vertex: (grid_pos_x, grid_pos_y - height_half)
		{{(float)grid_pos_x, (float)(grid_pos_y - height_half)}, tiling_colour, {0, 0}},
		// Right vertex: (grid_pos_x + width_half, grid_pos_y)
		{{(float)(grid_pos_x + width_half), (float)grid_pos_y}, tiling_colour, {0, 0}},
		// Bottom vertex: (grid_pos_x, grid_pos_y + height_half)
		{{(float)grid_pos_x, (float)(grid_pos_y + height_half)}, tiling_colour, {0, 0}},
		// Left vertex: (grid_pos_x - width_half, grid_pos_y)
		{{(float)(grid_pos_x - width_half), (float)grid_pos_y}, tiling_colour, {0, 0}}
	};

	// Render the geometry
	SDL_RenderGeometry(renderer, NULL, pos, sizeof(pos) / sizeof(pos[0]), indices, sizeof(indices) / sizeof(indices[0]));

	// Render farmhouse
	float x = grid_pos_x;
	float y = grid_pos_y;
	float w = 100 * scale;
	float h = texture_height / (texture_width / 100) * scale;

	SDL_FRect rect{
		x - (w / 2.0f), y - (h / 2.0f) - (offset_y * scale),
		w, h
	};
	SDL_RenderCopyF(renderer, texture, NULL, &rect);

	// Render resourse

	if (display_resource)
		resource_animation.render_icon(renderer, rect, scale);

	if (animate_collection)
		animate_collection = resource_animation.render_animation(renderer, scale);

	if (animate != Animate::Closed)
		render_information(renderer, scale);
}

void ResourceBuilding::render_information(SDL_Renderer* renderer, float scale)
{
	int screen_width, screen_height;
	SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

	float information_width_max = screen_width * 0.25;
	float information_width_speed = screen_width / 50;

	if (animate == Animate::Opening)
	{
		information_width += information_width_speed;
		information_width = fmin(information_width, information_width_max);

		if (information_width == information_width_max)
			animate = Animate::Opened;
	}
	if (animate == Animate::Closing)
	{
		information_width -= information_width_speed;
		information_width = fmax(information_width, 0);

		if (information_width == 0)
			animate = Animate::Closed;
	}

	if (animate != Animate::Closed)
	{
		SDL_FRect background_rect{
			screen_width - information_width, 0,
			information_width, screen_height
		};
		SDL_SetRenderDrawColor(renderer, 117, 71, 0, 255);
		SDL_RenderFillRectF(renderer, &background_rect);

		float x = (screen_width - information_width) + (information_width_max / 2);
		float y = screen_height / 12;
		FC_DrawAlign(font, renderer, x, y, FC_ALIGN_CENTER, "FARMHOUSE");

		for (int i = 0; i < 2; ++i)
		{
			SDL_Rect icon_rect{
				(screen_width - information_width) + (i * 50) + 15, screen_height / 6,
				50, 50
			};
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(renderer, &icon_rect);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(renderer, &icon_rect);
		}

		SDL_Rect info_rect{
			screen_width - information_width + 15, screen_height / 6 + 50,
			information_width_max - 30, screen_height - (screen_height / 6 + 50) - 15
		};

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, &info_rect);

		if (tab == InfoTab::Info)
		{
			FC_DrawAlign(info_tab_font, renderer, screen_width - information_width + 30, screen_height / 6 + 50, FC_ALIGN_LEFT,
				"With food as the foundation of any kingdom,\n"
				"the Farmhouse ensures your people have enough to live and fight on.");
		}
		else if (tab == InfoTab::Stats)
		{
			FC_DrawAlign(info_tab_font, renderer, screen_width - information_width + 30, screen_height / 6 + 50, FC_ALIGN_LEFT,
				"Production Rate / Hour %d\n"
				"Production Capacity %d",
				resource_per_sec, resource_capacity);
		}
	}
}

void ResourceBuilding::update()
{
	resource_amount = fmin(resource_capacity, resource_amount + resource_per_sec);
}

Uint32 ResourceBuilding::resource_callback(Uint32 interval, void* obj)
{
	auto* farmhouse = static_cast<ResourceBuilding*>(obj);
	
	farmhouse->resource_amount += farmhouse->resource_per_sec;
	farmhouse->display_resource = farmhouse->resource_amount >= 50;

	return interval;
}

ResourceBuildingType ResourceBuilding::get_type() const { return type; }

int ResourceBuilding::get_tiles_x() const { return tiles_x; }

int ResourceBuilding::get_tiles_y() const { return tiles_y; }

float ResourceBuilding::get_width(float scale) const { return tiles_x * (35 * scale); }

float ResourceBuilding::get_height(float scale) const { return tiles_y * (25 * scale); }

float ResourceBuilding::get_pos_x() const
{
	return grid_pos_x;
}

float ResourceBuilding::get_pos_y() const
{
	return grid_pos_y;
}

void ResourceBuilding::update_grid_pos(float scale)
{
	float left = actual_pos_x - (35 * scale) * (tiles_x / 2.0f) + (17.5f * scale);
	float min_dist = std::numeric_limits<float>::max();

	for (int i = tiles_y - 1; i < grid.side_length; ++i)
	{
		for (int j = 0; j < grid.side_length - tiles_x + 1; ++j)
		{
			float d = dist(grid.data[i][j].x, grid.data[i][j].y, left, actual_pos_y);

			if (d < min_dist)
			{
				min_dist = d;
				grid_pos_x = grid.data[i][j].x + (35 * scale) * (tiles_x / 2.0f) - (17.5f * scale);
				grid_pos_y = grid.data[i][j].y;
			}
		}
	}
}

void ResourceBuilding::set_tiling_colour(SDL_Color const& colour)
{
	tiling_colour = colour;
}
