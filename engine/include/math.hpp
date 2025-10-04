#pragma once

#include "SDL.h"


bool is_point_in_rectangle(
	float cx, float cy,
	SDL_FRect const& rect
);

bool is_point_in_rhombus(
	double cx, double cy,
	double w, double h,
	double mx, double my
);

bool is_rhombus_in_rhombus(
	float cx1, float cy1, float w1, float h1,
	float cx2, float cy2, float w2, float h2
);
