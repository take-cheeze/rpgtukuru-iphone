/**
 * @file
 * @brief Font Library
 * @author project.kuto
 */

#include <vector>

#include <kuto/kuto_font.h>
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_gl.h>
#include <kuto/kuto_types.h>

#include <font/Font.hpp>

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
	// kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();

	const void* (*fontFunc)(const char* input);
	std::string stdStr(str);

	switch(type) {
		case rpg2kLib::font::MINCHO:
			fontFunc = rpg2kLib::font::Font::getMincho;
			break;
		case rpg2kLib::font::GOTHIC:
		default:
			fontFunc = rpg2kLib::font::Font::getGothic;
			break;
	}

/*
	while(*str != NULL) {
		if( rpg2kLib::font::Font::isFullChar(str) ) {
			const void* f = fontFunc(str);

			for(int y = 0; y < rpg2kLib::font::FONT_H; y++) {
				uint mask = 0x01 << y;
				for(int x = 0; x < rpg2kLib::font::FULL_FONT_H; x++) {
					
				}
			}

			str++;
			str++;
		} else if( rpg2kLib::font::Font::isHalfChar(str) ) {
			const void* f = fontFunc(str);

			for(int y = 0; y < rpg2kLib::font::FONT_H; y++) {
				uint mask = 0x01 << y;
				for(int x = 0; x < rpg2kLib::font::HALF_FONT_H; x++) {
				
				}
			}

			str++;
		} else str++;
	}
 */
}

kuto::Vector2 Font::getTextSize(const char* str, float size, FONT_TYPE type)
{
	return kuto::Vector2( rpg2kLib::font::Font::width(str), rpg2kLib::font::FONT_H );
	// return kuto::Vector2(0.f, 0.f);
}

}	// namespace kuto
