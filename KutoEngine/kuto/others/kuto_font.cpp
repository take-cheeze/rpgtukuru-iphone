/**
 * @file
 * @brief Font Library
 * @author project.kuto
 */

#include <kuto/kuto_array.h>
#include <kuto/kuto_font.h>
#include <kuto/kuto_gl.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_types.h>
#include <kuto/kuto_utility.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>

#include <stdexcept>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <rpg2k/Encode.hpp>

/* TODO: special font
 * "$[a-zA-Z]"
 * "$$" -> "$"
 */


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
		{
			bitmapBuffer.zeromemory();
			// generate texture
			glGenTextures(1, &texture); kuto_assert(texture != GL_INVALID_VALUE);
			kuto::GraphicsDevice::instance().setTexture2D(true, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
				0, GL_ALPHA, GL_UNSIGNED_BYTE, &(bitmapBuffer[0]));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			currentX = 0;
		}
		~FontTexture()
		{
			if (texture != GL_INVALID_VALUE) glDeleteTextures(1, &texture);
		}
		void redrawTexture() {
			// redraw texture
			kuto::GraphicsDevice::instance().setTexture2D(true, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA
			, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT
			, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &(bitmapBuffer[0]));
		}

	public:
		kuto::Array<uint8_t, FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT>	bitmapBuffer;
		GLuint 					texture;
		std::vector<FontInfo>	fontInfoList;
		int						currentX;
		u32						minCode;
		u32						maxCode;
	};


	class FontImageCreater
	{
	private:
		static FT_Library library()
		{
			static struct Library
			{
				FT_Library library;

				Library() { if( FT_Init_FreeType(&library) != 0 ) kuto_assert(false); }
				~Library() { if( FT_Done_FreeType(library) != 0 ) kuto_assert(false); }
			} lib;
			return lib.library;
		}
		struct Face
		{
			FT_Face face;

			operator FT_Face&() { return face; }
			FT_Face operator ->() { return face; }
			Face(char const* name) { if( FT_New_Face(library(), name, 0, &face) != 0 ) kuto_assert(false); }
			~Face() { if( FT_Done_Face(face) != 0 ) kuto_assert(false); }
		};
	public:
		FontImageCreater(Font::Type const type)
		: currentTexture_(-1), face_( new Face(FONT_NAME[type]) )
		{
		}
		FontImageCreater(FontImageCreater const& src)
		: textureList_(src.textureList_)
		, currentTexture_(src.currentTexture_), face_(src.face_)
		{
		}

		void drawTextCode(u32 code, const kuto::Vector2& position, const kuto::Color& color)
		{
			FontTexture& fontTexture = textureList_[currentTexture_];

			if( FT_Set_Pixel_Sizes((*face_), FONT_BASE_SIZE, FONT_BASE_SIZE) != 0 ) kuto_assert(false);
			if( FT_Select_Charmap((*face_), FT_ENCODING_UNICODE) != 0 ) kuto_assert(false);

			FT_Matrix matrix = { 0x10000, 0, 0, 0x10000 };
			FT_Vector pen = { 0, 0 };

			FT_Set_Transform((*face_), &matrix, &pen);

			if( FT_Load_Char((*face_), code, FT_LOAD_TARGET_NORMAL) != 0 ) kuto_assert(false);
			FT_Glyph glyph_normal;
			if( FT_Get_Glyph((*face_)->glyph, &glyph_normal) != 0 ) kuto_assert(false);
			FT_Glyph_To_Bitmap(&glyph_normal, FT_RENDER_MODE_NORMAL, 0, 1);

			FT_BitmapGlyph glyph = (FT_BitmapGlyph)glyph_normal;

			int offsetX = glyph->left, offsetY = FONT_BASE_SIZE - glyph->top;

			for (int y = 0; y < glyph->bitmap.rows; y++) {
				for (int x = 0; x < glyph->bitmap.width; x++) {
					unsigned int pixel =
						((fontTexture.currentX + offsetX + x) + (offsetY + y) * FONT_TEXTURE_WIDTH);
					if( pixel >= fontTexture.bitmapBuffer.size() ) continue;
					fontTexture.bitmapBuffer[pixel] = glyph->bitmap.buffer[x + y * glyph->bitmap.width];
				}
			}

			FT_Done_Glyph(glyph_normal);

			fontTexture.redrawTexture();
		}

		Vector2 textCodeSize(u32 code, float scale)
		{
			FT_Error res;
			if( FT_Set_Pixel_Sizes((*face_), FONT_BASE_SIZE, FONT_BASE_SIZE) != 0 ) kuto_assert(false);
			if( FT_Select_Charmap((*face_), FT_ENCODING_UNICODE) != 0 ) kuto_assert(false);

			if( FT_Load_Char((*face_), code, FT_LOAD_TARGET_NORMAL) != 0 ) kuto_assert(false);
			FT_Glyph glyph = NULL;
			if( FT_Get_Glyph((*face_)->glyph, &glyph) != 0 ) kuto_assert(false);

			FT_BBox bbox;
			FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);

			FT_Done_Glyph(glyph);

			return Vector2(bbox.xMax, bbox.yMax) * scale;
		}

		FontInfo const& fontInfo(u32 code)
		{
			for (uint texIndex = 0; texIndex < textureList_.size(); texIndex++) {
				std::vector<FontInfo>& fontInfoList = textureList_[texIndex].fontInfoList;
				for (u32 i = 0; i < fontInfoList.size(); i++) {
					if (code == fontInfoList[i].code) {
						return fontInfoList[i];
					}
				}
			}

			//int codeLen = (code & 0x80)? 3:1;
			FontInfo info;
			info.code = code;
			info.width = textCodeSize(code, 1.f).x;
			if (textureList_.empty()
			|| textureList_[currentTexture_].currentX + info.width > FONT_TEXTURE_WIDTH) {
				textureList_.push_back( std::auto_ptr<FontTexture>( new FontTexture() ) );
				currentTexture_++;
			}
			FontTexture& fontTexture = textureList_[currentTexture_];
			info.x = (float)fontTexture.currentX;
			drawTextCode(code, kuto::Vector2(info.x, 0.f), kuto::Color(1.f, 1.f, 1.f, 1.f));
			info.texture = fontTexture.texture;
			fontTexture.fontInfoList.push_back(info);
			fontTexture.currentX += (int)(info.width + 1.5f);
			return fontTexture.fontInfoList.back();
		}

	private:
		boost::ptr_vector<FontTexture> textureList_;
		int currentTexture_;
		boost::shared_ptr<Face> face_;
	} fontImageCreater[] = { FontImageCreater(Font::GOTHIC), FontImageCreater(Font::MINCHO), };

	class Converter
	{
	private:
		::iconv_t cd_;
		static size_t const BUF_SIZE = 1024;
	public:
		Converter(std::string const& to, std::string const& from)
		: cd_(NULL)
		{
			cd_ = ::iconv_open( to.c_str(), from.c_str() );
			assert( cd_ != ::iconv_t(-1) );

			(void)(*this)("Convert test"); // skipping BOM
		}
		~Converter()
		{
			if( ::iconv_close(cd_) != 0 ) kuto_assert(false);
		}

		std::string operator()(std::string const& src) const
			throw(std::runtime_error)
		{
			char buf[BUF_SIZE];
			size_t inSize = src.size(), outSize = BUF_SIZE;
			#if RPG2K_IS_PSP && !defined(_LIBICONV_H)
				char const* inBuf  = src.c_str();
			#else
				char* inBuf  = const_cast< char* >( src.c_str() );
			#endif
			char* outBuf = buf;

			if( ::iconv(cd_, &inBuf, &inSize, &outBuf, &outSize) == size_t(-1) ) {
				throw std::runtime_error("convert error");
			} else return std::string(buf, BUF_SIZE - outSize);
		}
	} conv_("UTF-32", rpg2k::Encode::instance().systemEncoding());
} // namespace


Font::Font()
{
}

Font::~Font()
{
}

void Font::drawText(const char* str, const Vector2& pos, const Color& color, float size, Font::Type type)
{
	GraphicsDevice& device = GraphicsDevice::instance();

	GLfloat uvs[] = {
		0.f, 0.f,
		0.f, 1.f,
		1.f, 0.f,
		1.f, 1.f,
	};
	device.setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device.setTexCoordPointer(2, GL_FLOAT, 0, uvs);
	device.setVertexState(true, false, true, false);
	device.setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device.setColor(color);
	float x = pos.x;
	float const sizeRatio = size / FONT_BASE_SIZE;
	std::string const strUtf32 = conv_(str);
	for (uint i = 0; i < strUtf32.length() / sizeof(uint32_t); i++) {
		uint32_t const code = *reinterpret_cast<uint32_t const*>( &strUtf32[sizeof(uint32_t) * i] );
		FontInfo const& info = fontImageCreater[type].fontInfo(code);
		device.setTexture2D(true, info.texture);

		uvs[0] = info.x / FONT_TEXTURE_WIDTH; uvs[1] = 1.f;
		uvs[2] = info.x / FONT_TEXTURE_WIDTH; uvs[3] = 0.f;
		uvs[4] = (info.x + info.width) / FONT_TEXTURE_WIDTH; uvs[5] = 1.f;
		uvs[6] = (info.x + info.width) / FONT_TEXTURE_WIDTH; uvs[7] = 0.f;

		float aligned = ( (info.width * sizeRatio / size) > 0.55f )? size : (size * 0.5f);
		Matrix mt, ms;
		mt.translation(Vector3(x + (aligned - info.width * sizeRatio) * 0.5f, pos.y + FONT_TEXTURE_HEIGHT * sizeRatio, 0.f));
		ms.scaling(Vector3(info.width * sizeRatio, FONT_TEXTURE_HEIGHT * sizeRatio, 1.f));
		Matrix m = ms * mt;
		device.setModelMatrix(m);
		device.drawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += aligned; // info.width * sizeRatio;
	}
}
void Font::drawText(const char* str, const Vector2& pos, Texture& tex, uint const color, Font::Type type, float const size)
{
#if RPG2K_IS_PSP
	drawText(str, pos, Color(1.f, 1.f, 1.f, 1.f), size, type);
#else
	GraphicsDevice& device = GraphicsDevice::instance();

	static const Vector2 FONT_COLOR_S(16.f, 16.f);
	static const int FONT_COLOR_ROW = 10;
	Vector2 fontP = Vector2( color % FONT_COLOR_ROW, color / FONT_COLOR_ROW + 3 ) * FONT_COLOR_S;
	static Vector2 const shadeP(16.f, 32.f);

	GLfloat texCoordFont[8];
	GLfloat texCoordColor[8];
	texCoordColor[0] = fontP.x / tex.orgWidth(); texCoordColor[1] = (fontP.y + FONT_COLOR_S.y) / tex.orgHeight();
	texCoordColor[2] = fontP.x / tex.orgWidth(); texCoordColor[3] = fontP.y / tex.orgHeight();
	texCoordColor[4] = (fontP.x + FONT_COLOR_S.x) / tex.orgWidth(); texCoordColor[5] = (fontP.y + FONT_COLOR_S.y) / tex.orgHeight();
	texCoordColor[6] = (fontP.x + FONT_COLOR_S.x) / tex.orgWidth(); texCoordColor[7] = fontP.y / tex.orgHeight();
	GLfloat texCoordShade[8];
	texCoordShade[0] = shadeP.x / tex.orgWidth(); texCoordShade[1] = (shadeP.y + FONT_COLOR_S.y) / tex.orgHeight();
	texCoordShade[2] = shadeP.x / tex.orgWidth(); texCoordShade[3] = shadeP.y / tex.orgHeight();
	texCoordShade[4] = (shadeP.x + FONT_COLOR_S.x) / tex.orgWidth(); texCoordShade[5] = (shadeP.y + FONT_COLOR_S.y) / tex.orgHeight();
	texCoordShade[6] = (shadeP.x + FONT_COLOR_S.x) / tex.orgWidth(); texCoordShade[7] = shadeP.y / tex.orgHeight();

	device.setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device.setVertexState(true, false, true, false);
	device.setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	device.setColor(color);
	float x = pos.x;
	float const sizeRatio = size / FONT_BASE_SIZE;
	std::string const strUtf32 = conv_(str);

	device.setColor( Color(1.0f, 1.0f, 1.0f, 1.0f) );

	for (uint i = 0; i < strUtf32.length() / sizeof(uint32_t); i++) {
		uint32_t const code = *reinterpret_cast<uint32_t const*>( &strUtf32[sizeof(uint32_t) * i] );
		FontInfo const& info = fontImageCreater[type].fontInfo(code);
		texCoordFont[0] = info.x / FONT_TEXTURE_WIDTH; texCoordFont[1] = 1.f;
		texCoordFont[2] = info.x / FONT_TEXTURE_WIDTH; texCoordFont[3] = 0.f;
		texCoordFont[4] = (info.x + info.width) / FONT_TEXTURE_WIDTH; texCoordFont[5] = 1.f;
		texCoordFont[6] = (info.x + info.width) / FONT_TEXTURE_WIDTH; texCoordFont[7] = 0.f;

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex.glTexture());
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); // TODO: GL_COMBINE? GL_REPLACE?
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, info.texture);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_TEXTURE_2D);

		glClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoordShade);
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoordFont);

		float aligned = ( (info.width * sizeRatio / size) > 0.55f )? size : (size * 0.5f);
		{
			Matrix mt, ms;
			mt.translation(Vector3(x + (aligned - info.width * sizeRatio) * 0.5f, pos.y + FONT_TEXTURE_HEIGHT * sizeRatio, 0.f) + Vector3(1.f, 1.f, 0.f));
			ms.scaling(Vector3(info.width * sizeRatio, FONT_TEXTURE_HEIGHT * sizeRatio, 1.f));
			device.setModelMatrix(ms * mt);
		}
		device.drawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoordColor);
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoordFont);
		{
			Matrix mt, ms;
			mt.translation(Vector3(x, pos.y + FONT_TEXTURE_HEIGHT * sizeRatio, 0.f));
			ms.scaling(Vector3(info.width * sizeRatio, FONT_TEXTURE_HEIGHT * sizeRatio, 1.f));
			device.setModelMatrix(ms * mt);
		}
		device.drawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += aligned; // info.width * sizeRatio;
	}

	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	device.syncState();
#endif
}

kuto::Vector2 Font::textSize(const char* str, float size, Font::Type type)
{
	float width = 0.f;
	float height = 0.f;
	float const scale = size / FONT_BASE_SIZE;
	std::string const strUtf32 = conv_(str);
	for (uint i = 0; i < strUtf32.length() / sizeof(uint32_t); i++) {
		uint32_t const code = *reinterpret_cast<uint32_t const*>( &strUtf32[i * sizeof(uint32_t)] );

		Vector2 const v = fontImageCreater[type].textCodeSize(code, scale);
		width += ( (v.x / size) > 0.5f )? size : (size * 0.5f); // align // v.x;
		if(v.x > height) height = v.y;
	}
	return Vector2(width, height);
}

}	// namespace kuto
