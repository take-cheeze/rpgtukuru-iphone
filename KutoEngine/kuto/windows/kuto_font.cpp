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

#include <windows.h>


namespace kuto {

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


class FontTexture
{
public:
	FontTexture() {
		bitmapBuffer = (char*)malloc(FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT);
		std::memset(bitmapBuffer, 0, FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT);

		kuto::GraphicsDevice* device = kuto::GraphicsDevice::instance();
		// generate texture
		texture = 0;
		glGenTextures(1, &texture);
		device->setTexture2D(true, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
			0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmapBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		currentX = 0;
	}
	~FontTexture() {
		if (texture)
			glDeleteTextures(1, &texture);
		free(bitmapBuffer);
		bitmapBuffer = NULL;
	}
	void redrawTexture() {
		kuto::GraphicsDevice* device = kuto::GraphicsDevice::instance();
		// redraw texture
		device->setTexture2D(true, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
			0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmapBuffer);
	}

public:
	char*					bitmapBuffer;
	GLuint 					texture;
	std::vector<FontInfo>	fontInfoList;
	int						currentX;
	u32						minCode;
	u32						maxCode;
};


class FontImageCreater
{
public:
	FontImageCreater()
	: currentTexture(-1)
	{
		hFont = CreateFont(
			(int)FONT_BASE_SIZE, 0 , 0 , 0 , FW_REGULAR , FALSE , FALSE , FALSE ,
			SHIFTJIS_CHARSET , OUT_TT_ONLY_PRECIS ,
			CLIP_DEFAULT_PRECIS , DEFAULT_QUALITY ,
			FIXED_PITCH | FF_MODERN , "�l�r �S�V�b�N"
		);
	}

	~FontImageCreater()
	{
		for (u32 i = 0; i < fontTextureList.size(); i++)
			delete fontTextureList[i];
	}

	void drawTextCode(u32 code, const kuto::Vector2& position, float fontSize, const kuto::Color& color)
	{
		FontTexture& fontTexture = *fontTextureList[currentTexture];
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		TEXTMETRIC TM;
		GetTextMetrics( hdc, &TM );
		GLYPHMETRICS GM;
		CONST MAT2 Mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY8_BITMAP, &GM, 0, NULL, &Mat);
		BYTE *ptr = new BYTE[size];
		//std::memset(ptr, 255, size);
		GetGlyphOutline(hdc, code, GGO_GRAY8_BITMAP, &GM, size, ptr, &Mat);

		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);

		int width = (GM.gmBlackBoxX + 3) & 0xfffc;
		int offsX = GM.gmptGlyphOrigin.x;
		int offsY = TM.tmAscent - GM.gmptGlyphOrigin.y;
		for (int y = 0; y < (int)GM.gmBlackBoxY; y++) {
			for (int x = 0; x < width; x++) {
				char alpha = (char)((int)ptr[x + y * width] * 255 / 65);
				fontTexture.bitmapBuffer[fontTexture.currentX + x + offsX + (y + offsY) * FONT_TEXTURE_WIDTH] = alpha;
			}
		}
		delete[] ptr;
		//printf("%c, %d, %d, %d, %d\n", (char)code, (int)GM.gmCellIncX, (int)TM.tmHeight, (int)size, fontTexture.currentX);

		fontTexture.redrawTexture();
	}

	kuto::Vector2 textCodeSize(u32 code, float scale)
	{
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
		TEXTMETRIC TM;
		GetTextMetrics( hdc, &TM );
		GLYPHMETRICS GM;
		CONST MAT2 Mat = {{0,1},{0,0},{0,0},{0,1}};
		GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);
		return kuto::Vector2((float)GM.gmCellIncX * scale, (float)GM.gmBlackBoxY * scale);
	}

	const FontInfo& fontInfo(u32 code)
	{
		for (u32 texIndex = 0; texIndex < fontTextureList.size(); texIndex++) {
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
		info.width = textCodeSize(code, 1.f).x;
		if (fontTextureList.empty() || fontTextureList[currentTexture]->currentX + info.width > FONT_TEXTURE_WIDTH) {
			fontTextureList.push_back(new FontTexture());
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
	std::vector<FontTexture*>	fontTextureList;
	int							currentTexture;
	HFONT						hFont;
};

static FontImageCreater* fontImageCreater = NULL;

}

Font::Font()
{
	fontImageCreater = new FontImageCreater();
}

Font::~Font()
{
	delete fontImageCreater;
}

void Font::drawText(const char* str, const Vector2& pos, const Color& color, float size, FONT_TYPE type)
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
	std::string strUtf8 = utf82sjis(str);
	for (int i = 0; i < min((int)strUtf8.length(), 512); i++) {
		u32 code = (u32)strUtf8[i] & 0xFF;
		if (code & 0x80) {
			i++;
			code = ((code << 8) & 0xFF00) | ((u32)strUtf8[i] & 0xFF);
		}
		const FontInfo& info = fontImageCreater->fontInfo(code);
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

kuto::Vector2 Font::textSize(const char* str, float size, FONT_TYPE type)
{
	float width = 0.f;
	float height = 0.f;
	std::string strUtf8 = utf82sjis(str);
	for (int i = 0; i < min((int)strUtf8.length(), 512); i++) {
		u32 code = (u32)strUtf8[i] & 0xFF;
		if (code & 0x80) {
			i++;
			code = ((code << 8) & 0xFF00) | ((u32)strUtf8[i] & 0xFF);
		}
		Vector2 v = fontImageCreater->textCodeSize(code, size / FONT_BASE_SIZE);
		width += v.x;
		height = v.y;
	}
	return Vector2(width, height);
}

}	// namespace kuto
