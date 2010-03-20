/**
 * @file
 * @brief Font Library
 * @author project.kuto
 */

#include <vector>
#include <kuto/kuto_font.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_gl.h>
#include <kuto/kuto_types.h>

namespace {

// Vertex Array
const GLfloat panelVertices[] = {
     0,  0, // LeftTop
     0, -1, // LeftBottom
     1,  0, // RightTop
     1, -1, // RightBottom
};

// Vertex Array
const GLfloat panelUVs[] = {
     0,  0, // LeftTop
     0,  1, // LeftBottom
     1,  0, // RightTop
     1,  1, // RightBottom
};

const int FONT_TEXTURE_WIDTH = 1024;
const int FONT_TEXTURE_HEIGHT = 32;
const float FONT_BASE_SIZE = 24.0f;

struct FontInfo
{
	u32			code;
	float		width;
	float		x;
	GLint		texture;
};

}


namespace kuto {

Font::Font()
{
}

Font::~Font()
{
}

void Font::drawText(const char* str, const Vector2& pos, const Color& color, float size, FONT_TYPE type)
{
}

kuto::Vector2 Font::getTextSize(const char* str, float size, FONT_TYPE type)
{
	return kuto::Vector2(0.f, 0.f);
}

}	// namespace kuto
