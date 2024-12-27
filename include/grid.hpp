#pragma once

#include "object.hpp"

#include "SDL.h"

/*
 * A grid is a 2D matrix of rhombuses. Each rhombus is represented as a floating
 * point of its center.
 * 
 *        <>
 *      <> <>
 *     <> <> <>
 *      <> <>
 *        <>
 */
class Grid : private Object
{
public:
	Grid(int _side_length);
	~Grid();

public:
	void mouse_wheel(int mouse_x, int mouse_y, float scale_ratio);
	void mouse_drag(float dx, float dy);
	void render(SDL_Renderer* renderer, float scale);

public:
	int side_length;
	SDL_FPoint** data;
};