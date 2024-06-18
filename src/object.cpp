#include "object.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <vector>

void king::Object::pan_point(SDL_FPoint& point, float dx, float dy)
{
	point.x += dx;
	point.y += dy;
}

king::Farmhouse::Farmhouse(SDL_Renderer* renderer, std::array<SDL_Vertex, 4> const& _grid_snap_vertices)
	: texture(NULL), texture_width(-1), texture_height(-1), clr({ 0, 0, 0, 0 })
	, grid_snap_vertices(_grid_snap_vertices), absolute_vertices(_grid_snap_vertices)
	, start_mouse_drag_x(-1), start_mouse_drag_y(-1)
	, old_scale(1)
{
	// Load image using SDL_image
	SDL_Surface* surface = IMG_Load("../../assets/farmhouse.png");
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
}

king::Farmhouse::~Farmhouse()
{
	//SDL_DestroyTexture(texture);
}

void king::Farmhouse::pan(float dx, float dy)
{
	for (auto& vertex : grid_snap_vertices)
		pan_point(vertex.position, dx, dy);

	for (auto& vertex : absolute_vertices)
		pan_point(vertex.position, dx, dy);
}

bool king::Farmhouse::mouse_pressed(float mx, float my)
{
	if (!isPointInObject(mx, my))
		return false;
	
	Farmhouse::drag_ptr = this;

	clr = SDL_Colour{ 0, 255, 0, 255 };

	start_mouse_drag_x = mx;
	start_mouse_drag_y = my;

	start_grid_snap_vertices = grid_snap_vertices;
	start_absolute_vertices = absolute_vertices;

	return true;
}

void king::Farmhouse::drag(float mx, float my, std::forward_list<Farmhouse> const& farmhouses)
{
	int dx = mx - start_mouse_drag_x;
	int dy = my - start_mouse_drag_y;

	for (int i = 0; i < grid_snap_vertices.size(); ++i)
	{
		absolute_vertices[i].position.x += dx;
		absolute_vertices[i].position.y += dy;

		float ws = 35 * old_scale;
		float hs = 25 * old_scale;

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

	start_mouse_drag_x = mx;
	start_mouse_drag_y = my;

	// Update colour

	clr = SDL_Colour{ 0, 255, 0, 255 };

	for (auto const& farmhouse : farmhouses)
	{
		if (&farmhouse != this &&
			farmhouse.isPolygonInObject(this->grid_snap_vertices))
		{
			clr = SDL_Colour{ 255, 0, 0, 255 };
			break;
		}
	}
}

void king::Farmhouse::mouse_released()
{
	if (clr.r == 255)
	{
		grid_snap_vertices = start_grid_snap_vertices;
		absolute_vertices = start_absolute_vertices;
	}

	clr = SDL_Colour{ 0, 0, 0, 0 };
}

void king::Farmhouse::zoom(SDL_MouseWheelEvent const& wheel)
{
	int scroll = wheel.y;
	float new_scale = old_scale + scroll * 0.1f;

	// Prevent scrolling too close or far
	if (new_scale < 0.5f || new_scale > 6.f)
		return;

	// Find offset from position to mouse
	// Multiply that by new scale to get new offset
	// Divide by old scale to get relative offset?? - that what gpt4o said and it worked
	// Update the new position to be from the offset of mouse

	int mouse_x = wheel.mouseX;
	int mouse_y = wheel.mouseY;

	for (auto& vertex : grid_snap_vertices)
	{
		vertex.position.x = (vertex.position.x - mouse_x) * new_scale / old_scale + mouse_x;
		vertex.position.y = (vertex.position.y - mouse_y) * new_scale / old_scale + mouse_y;
	}

	for (auto& vertex : absolute_vertices)
	{
		vertex.position.x = (vertex.position.x - mouse_x) * new_scale / old_scale + mouse_x;
		vertex.position.y = (vertex.position.y - mouse_y) * new_scale / old_scale + mouse_y;
	}

	old_scale = new_scale;
}

void king::Farmhouse::render(SDL_Renderer* renderer)
{
	for (auto& vertex : grid_snap_vertices)
		vertex.color = clr;

	// Render green grid base
	int indices[] = {
		0, 2, 3, // First triangle (Top, Right, Bottom)
		0, 1, 3  // Second triangle (Top, Bottom, Left)
	};
	if (SDL_RenderGeometry(renderer, NULL, grid_snap_vertices.data(), grid_snap_vertices.size(), indices, 6) == -1)
		SDL_Log("Failed to render farmhouse grid base: %s\n", SDL_GetError());

	// Render farmhouse
	float w = 200 * old_scale;
	float h = texture_height / (texture_width / 200) * old_scale;

	int x = (grid_snap_vertices[0].position.x + grid_snap_vertices[3].position.x) / 2;
	int y = (grid_snap_vertices[1].position.y + grid_snap_vertices[2].position.y) / 2;

	SDL_FRect dest_rect = {
			x - (w / 2.0), y - (h / 2.0) - 32.5f * old_scale,
			w, h
	};

	if (SDL_RenderCopyF(renderer, texture, NULL, &dest_rect) == -1)
		SDL_Log("Failed to render farmhouse texture: %s\n", SDL_GetError());
}

bool king::Farmhouse::isPointInObject(int px, int py) const
{
	auto crossProduct = [](float ax, float bx, float ay, float by) {
		return ax * by - ay * bx;
	};

	int cx = (grid_snap_vertices[0].position.x + grid_snap_vertices[3].position.x) / 2;
	int cy = (grid_snap_vertices[1].position.y + grid_snap_vertices[2].position.y) / 2;
	int w = 35 * 7 * old_scale;
	int h = 25 * 7 * old_scale;

	// Define the vertices of the rhombus
	SDL_FPoint points[4] = {
		{cx,         cy - h / 2},  // Top
		{cx + w / 2, cy        },  // Right
		{cx,         cy + h / 2},  // Bottom
		{cx - w / 2, cy        }   // Left
	};

	// Calculate vectors from the vertices to the point
	float vectors[4][2] = {
		{px - points[0].x, py - points[0].y},
		{px - points[1].x, py - points[1].y},
		{px - points[2].x, py - points[2].y},
		{px - points[3].x, py - points[3].y}
	};

	// Calculate cross products for each pair of vectors
	float crossProducts[4] = {
		crossProduct(points[1].x - points[0].x, points[1].y - points[0].y, vectors[0][0], vectors[0][1]),
		crossProduct(points[2].x - points[1].x, points[2].y - points[1].y, vectors[1][0], vectors[1][1]),
		crossProduct(points[3].x - points[2].x, points[3].y - points[2].y, vectors[2][0], vectors[2][1]),
		crossProduct(points[0].x - points[3].x, points[0].y - points[3].y, vectors[3][0], vectors[3][1])
	};

	// Check if all cross products have the same sign
	bool allPositive = true;
	bool allNegative = true;

	for (int i = 0; i < 4; ++i) {
		if (crossProducts[i] < 0) allPositive = false;
		if (crossProducts[i] > 0) allNegative = false;
	}

	return allPositive || allNegative;
}


// POLYGON/POINT
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

bool king::Farmhouse::isPolygonInObject(std::array<SDL_Vertex, 4> const& _vertices) const
{
	// assert(_vertices.size() == 4); // rhombus

	// Check for vertex containment
	for (int i = 0; i < 4; ++i)
	{
		if (is_point_in_rhombus(grid_snap_vertices, _vertices[i].position.x, _vertices[i].position.y) ||
			is_point_in_rhombus(_vertices, grid_snap_vertices[i].position.x, grid_snap_vertices[i].position.y))
			return true;
	}

	return false;
}