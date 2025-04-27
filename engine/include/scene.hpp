#pragma once

#include <cstdint>

#include "SDL.h"

class Scene
{
public:
	Scene(SDL_Window* _window, SDL_Renderer* _renderer);

	/*
	 * Mouse wheel.
	 */
	virtual void handle_scroll(int scroll) = 0;

	/*
	 * Mouse press and release taking less than 200 ms
	 */
	virtual void handle_mouse_click(int mouse_x, int mouse_y) = 0;

	/*
	 * Mouse press and release taking more than 200 ms
	 */
	virtual void handle_mouse_drag_start(int mouse_x, int mouse_y) = 0;

	/*
	 * 
	 */
	virtual void handle_mouse_drag_duration(int mouse_x, int mouse_y) = 0;

	/*
	 * Mouse release after mouse press for more than 200 ms.
	 */
	virtual void handle_mouse_drag_end(int mouse_x, int mouse_y) = 0;

	/*
	 * 
	 */
	virtual void update() = 0;

	/*
	 * 
	 */
	virtual void render() = 0;

	/*
	 *
	 */
	float get_scale() const;

public:
	static Scene* current_scene;
	static Scene* scenes[5];

protected:
	SDL_Window* window;
	SDL_Renderer* renderer;

	float scale;
};