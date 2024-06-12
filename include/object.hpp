#pragma once

#include "SDL.h"

#include <array>
#include <vector>

namespace king {

struct Object
{
	float x, y;
	float old_scale;
	SDL_Colour clr;

	Object(float _x, float _y);
	virtual ~Object();

	virtual void pan(float dx, float dy);
	virtual void drag(float dx, float dy);
	virtual void zoom(SDL_MouseWheelEvent const& wheel);
	virtual void render(SDL_Renderer* renderer) const;

	virtual bool isPointInObject(int px, int py) const;

private:
	void pan_point(float& x, float dx, float dy);
};

struct Farmhouse : public Object
{
	Farmhouse(SDL_Renderer* renderer, std::array<SDL_Vertex, 4> const& _grid_snap_vertices);
	~Farmhouse();

	SDL_Texture* texture;
	int texture_width, texture_height;

	std::array<SDL_Vertex, 4> grid_snap_vertices;
	std::array<SDL_Vertex, 4> absolute_vertices;

	void pan(float dx, float dy) override;
	void drag(float dx, float dy) override;
	void zoom(SDL_MouseWheelEvent const& wheel) override;
	void render(SDL_Renderer* renderer) const override;

	bool isPointInObject(int px, int py) const override;
	bool isPolygonInObject(std::array<SDL_Vertex, 4> const& _vertices) const;
};

} // namespace king