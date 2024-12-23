#pragma once

#include <cstdint>

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
	virtual void handle_mouse_click() = 0;

	/*
	 * Mouse press and release taking more than 200 ms
	 */
	virtual void handle_mouse_drag_start() = 0;

	/*
	 * Mouse release after mouse press for more than 200 ms.
	 */
	virtual void handle_mouse_drag_end() = 0;

	/*
	 * Switches the scene to next_id.
	 */
	virtual void switch_scene(scene_id next_id) = 0;

private:
	scene_id id;
};