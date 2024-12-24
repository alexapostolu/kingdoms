#pragma once

#include <cstdint>

#include "SDL.h"

/*
 * Unique identifier for each scene.
 */
using scene_id = uint8_t;

class Scene
{
public:
	/*
	 * Mouse wheel.
	 */
	virtual void handle_scroll() = 0;

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
	 * Switches the scene to next_id.
	 */
	virtual void switch_scene(scene_id next_id) = 0;

protected:
	float scale = 1;

private:
	static SDL_Window* window;
	static SDL_Renderer* renderer;

	scene_id id;
};