/**
 * @file
 * @brief Viewport
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"


namespace kuto {

class Viewport
{
public:
	Viewport() {}
	Viewport(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
	Viewport(const Viewport& vp) : x(vp.x), y(vp.y), width(vp.width), height(vp.height) {}

	void set(float x, float y, float width, float height) {
		this->x = x; this->y = y; this->width = width; this->height = height;
	}

public:
	float		x;
	float		y;
	float		width;
	float		height;
};	// class Viewport

}	// namespace kuto
