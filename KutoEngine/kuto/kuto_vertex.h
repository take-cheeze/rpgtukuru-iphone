/**
 * @file
 * @brief Vertex Format
 * @author takuto
 */
#pragma once

#include "kuto_math.h"


namespace kuto {

struct Vertex2DPos
{
	Vector2		position;
};

struct Vertex2DPosColor
{
	Vector2		position;
	Color		color;
};

struct Vertex2DPosUV
{
	Vector2		position;
	Vector2		uv;
};

struct Vertex2DPosColorUV
{
	Vector2		position;
	Color		color;
	Vector2		uv;
};

}	// namespace kuto
