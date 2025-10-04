#include "math.hpp"

#include <cmath>


bool is_point_in_rectangle(float cx, float cy, SDL_FRect const& rect)
{
	return cx > (rect.x - rect.w / 2) && cx < (rect.x + rect.w / 2) &&
		   cy > (rect.y - rect.h / 2) && cy < (rect.y + rect.h / 2);
}

bool is_point_in_rhombus(double cx, double cy, double w, double h, double mx, double my)
{
	double dx = std::abs(mx - cx);
	double dy = std::abs(my - cy);
	return (dx / (w / 2.0)) + (dy / (h / 2.0)) < 1.0;
}

bool is_rhombus_in_rhombus(
	float cx1, float cy1, float w1, float h1,
	float cx2, float cy2, float w2, float h2)
{
	// Quick bounding box test
	if (std::abs(cx1 - cx2) > (w1 + w2) / 2.0 || std::abs(cy1 - cy2) > (h1 + h2) / 2.0) {
		return false; // Bounding boxes don't overlap
	}

	// Vertices of Rhombus 1
	double v1x[] = { cx1 + w1 / 2.0, cx1 - w1 / 2.0, cx1, cx1 };
	double v1y[] = { cy1, cy1, cy1 + h1 / 2.0, cy1 - h1 / 2.0 };

	// Vertices of Rhombus 2
	double v2x[] = { cx2 + w2 / 2.0, cx2 - w2 / 2.0, cx2, cx2 };
	double v2y[] = { cy2, cy2, cy2 + h2 / 2.0, cy2 - h2 / 2.0 };

	// Check if any vertex of Rhombus 1 is inside Rhombus 2
	for (int i = 0; i < 4; ++i) {
		if (is_point_in_rhombus(cx2, cy2, w2, h2, v1x[i], v1y[i])) {
			return true;
		}
	}

	// Check if any vertex of Rhombus 2 is inside Rhombus 1
	for (int i = 0; i < 4; ++i) {
		if (is_point_in_rhombus(cx1, cy1, w1, h1, v2x[i], v2y[i])) {
			return true;
		}
	}

	return false;
}