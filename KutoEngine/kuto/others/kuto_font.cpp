/**
 * @file
 * @brief Font Library
 * @author project.kuto
 */

#include <vector>
#include <kuto/kuto_font.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_gl.h>
#include <kuto/kuto_types.h>
#include <kuto/kuto_utility.h>

#include <boost/smart_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


namespace kuto {
namespace
{
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
	char const* FONT_NAME[] = {
		"IPAfont00302/ipag.ttf", // gothic
		"IPAfont00302/ipam.ttf", // mincho
	};

	struct FontInfo
	{
		u32			code;
		float		width;
		float		x;
		GLint		texture;
	};


	class FontTexture
	{
	public:
		FontTexture()
		: bitmapBuffer(FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT, 0)
		{
			kuto::GraphicsDevice* device = kuto::GraphicsDevice::instance();
			// generate texture
			texture = 0;
			glGenTextures(1, &texture);  
			device->setTexture2D(true, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
				0, GL_ALPHA, GL_UNSIGNED_BYTE, &(bitmapBuffer[0]));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			currentX = 0;
		}
		~FontTexture() { if (texture) glDeleteTextures(1, &texture); }
		void redrawTexture() {
			kuto::GraphicsDevice* device = kuto::GraphicsDevice::instance();
			// redraw texture
			device->setTexture2D(true, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
				0, GL_ALPHA, GL_UNSIGNED_BYTE, &(bitmapBuffer[0]));
		}

	public:
		std::vector< uint8_t >	bitmapBuffer;
		GLuint 					texture;
		std::vector<FontInfo>	fontInfoList;
		int						currentX;
		u32						minCode;
		u32						maxCode;
	};


	class FontImageCreater
	{
	private:
		static FT_Library getLibrary()
		{
			static struct Library
			{
				Library() { FT_Error res = FT_Init_FreeType(&library); res = res; assert(res == 0); }
				~Library() { FT_Error res = FT_Done_FreeType(library); res = res; assert(res == 0); }
				FT_Library library;
			} lib;
			return lib.library;
		}
	public:
		FontImageCreater(Font::Type type)
		: currentTexture(-1)
		{
			FT_Error res = FT_New_Face(getLibrary(), FONT_NAME[type], 0, &face_); assert(res == 0);
		}
		~FontImageCreater()
		{
			FT_Error res = FT_Done_Face(face_); assert(res == 0);
		}

		void drawTextCode(u32 code, const kuto::Vector2& position, float fontSize, const kuto::Color& color)
		{
			FT_Error res;

			FontTexture& fontTexture = *fontTextureList[currentTexture];

			res = FT_Set_Pixel_Sizes(face_, FONT_BASE_SIZE, FONT_BASE_SIZE); assert(res == 0);
			res = FT_Select_Charmap(face_, FT_ENCODING_UNICODE); assert(res == 0);

			FT_Matrix matrix;
			matrix.xx = 0x10000;
			matrix.xy = 0;
			matrix.yx = 0;
			matrix.yy = 0x10000;

			FT_Vector pen;
			pen.x = 0;
			pen.y = 0;

			FT_Set_Transform(face_, &matrix, &pen);

			res = FT_Load_Char(face_, code, FT_LOAD_TARGET_NORMAL); assert(res == 0);
			FT_Glyph glyph_normal;
			res = FT_Get_Glyph(face_->glyph, &glyph_normal); assert(res == 0);
			FT_Glyph_To_Bitmap(&glyph_normal, FT_RENDER_MODE_NORMAL, 0, 1);

			FT_BitmapGlyph glyph = (FT_BitmapGlyph)glyph_normal;

			pen.x += face_->glyph->advance.x;

			int width = 0;
			int height = 0;

			if(width < glyph->bitmap.width + glyph->left) width = glyph->bitmap.width + glyph->left;
			if(height < glyph->bitmap.rows + glyph->top) height = glyph->bitmap.rows + glyph->top;

			int min_y = height;
			if(min_y > height - glyph->top) min_y = height - glyph->top;
			min_y = (int)(((float)min_y + 0.5f) / 2.0f);

			int offsetX = glyph->left, offsetY = height - glyph->top;

			for (int y = 0; y < glyph->bitmap.rows; y++) {
				for (int x = 0; x < glyph->bitmap.width; x++) {
					if(offsetY + y - min_y < 0 || offsetX + x < 0) continue;
					if(offsetY + y - min_y > height || offsetX + x > width) continue;
					if((((offsetX + x) + (offsetY + y - min_y) * width) * 4) >= width * height * 4) continue;
					fontTexture.bitmapBuffer[fontTexture.currentX + x + offsetX + (y + offsetY) * FONT_TEXTURE_WIDTH] = glyph->bitmap.buffer[x + y * glyph->bitmap.width];
				}
			}

			FT_Done_Glyph(glyph_normal);

			fontTexture.redrawTexture();
		}

		Vector2 getTextCodeSize(u32 code, float scale)
		{
			FT_Error res;
			res = FT_Set_Pixel_Sizes(face_, FONT_BASE_SIZE, FONT_BASE_SIZE); assert(res == 0);
			res = FT_Select_Charmap(face_, FT_ENCODING_UNICODE); assert(res == 0);

			int width = 0;
			int height = 0;

			res = FT_Load_Char(face_, code, FT_LOAD_TARGET_NORMAL); assert(res == 0);
			FT_Glyph glyph = NULL;
			res = FT_Get_Glyph(face_->glyph, &glyph); assert(res == 0);

			FT_BBox bbox;
			FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);

			width += bbox.xMax - bbox.xMin;
			if (height < bbox.yMax - bbox.yMin) height = bbox.yMax - bbox.yMin;

			FT_Done_Glyph(glyph);

			return Vector2(width, height) * scale;
		}

		const FontInfo& getFontInfo(u32 code)
		{
			for (uint texIndex = 0; texIndex < fontTextureList.size(); texIndex++) {
				std::vector<FontInfo>& fontInfoList = fontTextureList[texIndex]->fontInfoList;
				for (u32 i = 0; i < fontInfoList.size(); i++) {
					if (code == fontInfoList[i].code) {
						return fontInfoList[i];
					}
				}
			}

			//int codeLen = (code & 0x80)? 3:1;
			FontInfo info;
			info.code = code;
			info.width = getTextCodeSize(code, 1.f).x;
			if (fontTextureList.empty() || fontTextureList[currentTexture]->currentX + info.width > FONT_TEXTURE_WIDTH) {
				fontTextureList.push_back( boost::shared_ptr< FontTexture >( new FontTexture() ) );
				currentTexture++;
			}
			FontTexture& fontTexture = *fontTextureList[currentTexture];
			info.x = (float)fontTexture.currentX;
			drawTextCode(code, kuto::Vector2(info.x, 0.f), FONT_BASE_SIZE, kuto::Color(1.f, 1.f, 1.f, 1.f));
			info.texture = fontTexture.texture;
			fontTexture.fontInfoList.push_back(info);
			fontTexture.currentX += (int)(info.width + 0.5f);
			return fontTexture.fontInfoList.back();
		}

	private:
		std::vector< boost::shared_ptr< FontTexture > >	fontTextureList;
		int							currentTexture;
		FT_Face						face_;
	} fontImageCreater[] = { FontImageCreater(Font::GOTHIC), FontImageCreater(Font::MINCHO), };
} // namespace


Font::Font()
{
}

Font::~Font()
{
}

void Font::drawText(const char* str, const Vector2& pos, const Color& color, float size, Font::Type type)
{
	GraphicsDevice* device = GraphicsDevice::instance();

	static GLfloat uvs[] = {
		0.f, 0.f,
		0.f, 1.f,
		1.f, 0.f,
		1.f, 1.f,
	};
	device->setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device->setTexCoordPointer(2, GL_FLOAT, 0, uvs);
	device->setVertexState(true, false, true, false);
	device->setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device->setColor(color);
	float x = pos.x;
	float sizeRatio = size / FONT_BASE_SIZE;
	std::string strUtf8 = /* utf82sjis */ sjis2utf8(str);
	for (int i = 0; i < min((int)strUtf8.length(), 512); i++) {
		u32 code = (u32)strUtf8[i] & 0xFF;
		if (code & 0x80) {
			i++;
			code = ((code << 8) & 0xFF00) | ((u32)strUtf8[i] & 0xFF);
		}
		const FontInfo& info = fontImageCreater[type].getFontInfo(code);
		device->setTexture2D(true, info.texture);

		uvs[0] = info.x / FONT_TEXTURE_WIDTH; uvs[1] = 1.f;
		uvs[2] = info.x / FONT_TEXTURE_WIDTH; uvs[3] = 0.f;
		uvs[4] = (info.x + info.width) / FONT_TEXTURE_WIDTH; uvs[5] = 1.f;
		uvs[6] = (info.x + info.width) / FONT_TEXTURE_WIDTH; uvs[7] = 0.f;

		Matrix mt, ms;
		mt.translation(Vector3(x, pos.y + FONT_TEXTURE_HEIGHT * sizeRatio, 0.f));
		ms.scaling(Vector3(info.width * sizeRatio, FONT_TEXTURE_HEIGHT * sizeRatio, 1.f));
		Matrix m = ms * mt;
		device->setModelMatrix(m);
		device->drawArrays(GL_TRIANGLE_STRIP, 0, 4);
		x += info.width * sizeRatio;
	}
}

kuto::Vector2 Font::getTextSize(const char* str, float size, Font::Type type)
{
	float width = 0.f;
	float height = 0.f;
	std::string strUtf8 = /* utf82sjis */ sjis2utf8(str);
	for (int i = 0; i < min((int)strUtf8.length(), 512); i++) {
		u32 code = (u32)strUtf8[i] & 0xFF;
		if (code & 0x80) {
			i++;
			code = ((code << 8) & 0xFF00) | ((u32)strUtf8[i] & 0xFF);
		}
		Vector2 v = fontImageCreater[type].getTextCodeSize(code, size / FONT_BASE_SIZE);
		width += v.x;
		height = v.y;
	}
	return Vector2(width, height);
}

}	// namespace kuto
