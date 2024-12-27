#include "resource_building.hpp"

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
	Grid const& grid,
	float _scale,
	FC_Font* _font
)
	: type(_type)
	, renderer(_renderer)
	, texture(nullptr), texture_width(-1), texture_height(-1), clr({ 0, 0, 0, 0 })
	, offset_x(-1), offset_y(-1)
	, grid_snap_vertices(), absolute_vertices()
	, start_mouse_drag_x(-1), start_mouse_drag_y(-1)
	, record_start_vertices(true), start_grid_snap_vertices(), start_absolute_vertices()
	, display_resource(false)
	, resource_amount(0), resource_per_sec(1), resource_capacity(500)
	, show_information(false), info_rect()
	, animate(Animate::Closed), information_width(0)
	, font(_font)
	, resource_animation(_type, _renderer), animate_collection(false)
	, info_tab_font(FC_CreateFont())
	, tab(InfoTab::Info)
{
	int screen_width, screen_height;
	SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

	std::string image;
	std::string resource_image;
	resource_building_type_to_image(_type, image, resource_image);

	/* Get texture */

	// Load image using SDL_image
	SDL_Surface* surface = IMG_Load(image.c_str());
	if (!surface)
	{
		SDL_Log("Failed to load image: %s\n", IMG_GetError());
		return;
	}

	texture_width = surface->w;
	texture_height = surface->h;

	// Convert surface to texture
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		SDL_Log("Failed to create texture: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(surface);

	/* Get grid position */

	// Get center grid tile
	int cx = 0, cy = 0;
	for (int i = 0; i < grid.height; ++i)
	{
		for (int j = 0; j < grid.width; ++j)
		{
			if (dist(grid.data[i][j].x * _scale, grid.data[i][j].y * _scale, pos.x, pos.y) <
				dist(grid.data[cy][cx].x * _scale, grid.data[cy][cx].y * _scale, pos.x, pos.y))
			{
				cx = j;
				cy = i;
			}
		}
	}

	grid_snap_vertices = {
		SDL_Vertex{ (grid.data[cy][cx].x + 0.0f) * _scale,	  (grid.data[cy][cx].y - 62.5f	) * _scale },	 // top
		SDL_Vertex{ (grid.data[cy][cx].x + 123.5f) * _scale, (grid.data[cy][cx].y + 25		) * _scale },	 // right
		SDL_Vertex{ (grid.data[cy][cx].x - 0.0f) * _scale,	  (grid.data[cy][cx].y + 112.5f	) * _scale },	 // bottom
		SDL_Vertex{ (grid.data[cy][cx].x - 123.5f) * _scale, (grid.data[cy][cx].y + 25		) * _scale } };	 // left

	offset_x = 0;
	offset_y = 25;

	absolute_vertices = grid_snap_vertices;

	info_rect.x = grid_snap_vertices[0].position.x;
	info_rect.y = grid_snap_vertices[0].position.y - 100;
	info_rect.w = 80;
	info_rect.h = 80;

	int font_size = screen_height / 50;
	FC_LoadFont(info_tab_font, renderer, "../../assets/Cinzel.ttf", font_size, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
}

void ResourceBuilding::init_resource_timer()
{
	SDL_AddTimer(3000, ResourceBuilding::resource_callback, this);
}

void ResourceBuilding::pan(float dx, float dy)
{
	for (auto& vertex : grid_snap_vertices)
		pan_point(vertex.position, dx, dy);

	for (auto& vertex : absolute_vertices)
		pan_point(vertex.position, dx, dy);
}

bool is_point_in_rhombus(std::array<SDL_Vertex, 4> const& vertices, float px, float py)
{
	bool collision = false;

	// go through each of the vertices, plus
	// the next vertex in the list
	int next = 0;
	for (int current = 0; current < vertices.size(); current++) {

		// get next vertex in list
		// if we've hit the end, wrap around to 0
		next = current + 1;
		if (next == vertices.size()) next = 0;

		// get the PVectors at our current position
		// this makes our if statement a little cleaner
		SDL_Vertex vc = vertices[current];    // c for "current"
		SDL_Vertex vn = vertices[next];       // n for "next"

		// compare position, flip 'collision' variable
		// back and forth
		if (((vc.position.y >= py && vn.position.y < py) || (vc.position.y < py && vn.position.y >= py)) &&
			(px < (vn.position.x - vc.position.x) * (py - vc.position.y) / (vn.position.y - vc.position.y) + vc.position.x)) {
			collision = !collision;
		}
	}
	return collision;
}

bool ResourceBuilding::mouse_press(float mx, float my)
{
	bool is_clicked = is_point_in_rhombus(grid_snap_vertices, mx, my);

	if (!is_clicked)
	{
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
	}

	return is_clicked;
}

void ResourceBuilding::mouse_press_update(float scale, unsigned int& wheat, unsigned int& wood)
{
	if (display_resource)
	{
		resource_animation.init_animation(renderer, grid_snap_vertices[0].position.x, grid_snap_vertices[1].position.y, scale, resource_amount);

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

bool ResourceBuilding::is_rhombus_in_rhombus(std::array<SDL_Vertex, 4> const& _vertices) const
{
	// Check for vertex containment
	for (int i = 0; i < 4; ++i)
	{
		if (is_point_in_rhombus(grid_snap_vertices, _vertices[i].position.x, _vertices[i].position.y) ||
			is_point_in_rhombus(_vertices, grid_snap_vertices[i].position.x, grid_snap_vertices[i].position.y))
			return true;
	}

	return false;
}

void ResourceBuilding::mouse_drag(float dx, float dy, std::forward_list<ResourceBuilding> const& farmhouses, float scale)
{
	if (record_start_vertices)
	{
		record_start_vertices = false;

		start_grid_snap_vertices = grid_snap_vertices;
		start_absolute_vertices = absolute_vertices;
	}

	for (int i = 0; i < grid_snap_vertices.size(); ++i)
	{
		absolute_vertices[i].position.x += dx;
		absolute_vertices[i].position.y += dy;

		float ws = 35 * scale;
		float hs = 25 * scale;

		float d = absolute_vertices[i].position.x - grid_snap_vertices[i].position.x;
		while (abs(d) >= ws)
		{
			if (d >= 0)
			{
				grid_snap_vertices[i].position.x += ws;
				d -= ws;
			}
			else
			{
				grid_snap_vertices[i].position.x -= ws;
				d += ws;
			}
		}
		d = absolute_vertices[i].position.y - grid_snap_vertices[i].position.y;
		while (abs(d) >= hs)
		{
			if (d >= 0)
			{
				grid_snap_vertices[i].position.y += hs;
				d -= hs;
			}
			else
			{
				grid_snap_vertices[i].position.y -= hs;
				d += hs;
			}
		}
	}

	// Update colour

	clr = SDL_Colour{ 0, 255, 0, 255 };

	for (auto const& farmhouse : farmhouses)
	{
		if (&farmhouse != this &&
			farmhouse.is_rhombus_in_rhombus(this->grid_snap_vertices))
		{
			clr = SDL_Colour{ 255, 0, 0, 255 };
			break;
		}
	}

	show_information = false;
}

bool ResourceBuilding::mouse_release()
{
	bool is_blocked = false;

	if (clr.r == 255)
	{
		grid_snap_vertices = start_grid_snap_vertices;
		absolute_vertices = start_absolute_vertices;
		
		is_blocked = true;
	}

	record_start_vertices = true;
	clr = SDL_Colour{ 0, 0, 0, 0 };

	show_information = !show_information;

	return is_blocked;
}

void ResourceBuilding::mouse_wheel(int mouse_x, int mouse_y, float scale_ratio)
{
	for (auto& vertex : grid_snap_vertices)
	{
		vertex.position.x = (vertex.position.x - mouse_x) * scale_ratio + mouse_x;
		vertex.position.y = (vertex.position.y - mouse_y) * scale_ratio + mouse_y;
	}

	for (auto& vertex : absolute_vertices)
	{
		vertex.position.x = (vertex.position.x - mouse_x) * scale_ratio + mouse_x;
		vertex.position.y = (vertex.position.y - mouse_y) * scale_ratio + mouse_y;
	}
}

void ResourceBuilding::render(SDL_Renderer* renderer, float scale)
{
	for (auto& vertex : grid_snap_vertices)
		vertex.color = clr;

	// Render green grid base
	int indices[] = {
		0, 1, 2, // Top, Right, Bottom
		0, 3, 2  // Top, Bottom, Left
	};
	SDL_RenderGeometry(renderer, NULL, grid_snap_vertices.data(), grid_snap_vertices.size(), indices, 6);

	// Render farmhouse
	float x = grid_snap_vertices[0].position.x;
	float y = grid_snap_vertices[1].position.y;
	float w = 200 * scale;
	float h = texture_height / (texture_width / 200) * scale;

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