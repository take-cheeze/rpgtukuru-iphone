/**
 * @file
 * @brief 2D Graphics
 * @author project.kuto
 */

#include "kuto_graphics2d.h"
#include "kuto_render_manager.h"
#include "kuto_vertex.h"
#include "kuto_graphics_device.h"
#include "kuto_file.h"


namespace
{
	// Vertex Array
	const GLfloat panelVertices[] = {
		 0,  0, // LeftTop
		 0, -1, // LeftBottom
		 1,  0, // RightTop
		 1, -1, // RightBottom
	};
	const GLfloat rotVertices[] = {
		-0.5f, 0.5f, // LeftTop
		-0.5f, -0.5f, // LeftBottom
		0.5f,  0.5f, // RightTop
		0.5f, -0.5f, // RightBottom
	};

	static GLfloat uvArray[8];
}

namespace kuto {

Graphics2D::Graphics2D(GraphicsDevice& dev)
: device_(dev)
{
}

void Graphics2D::drawText(const char* str, const Vector2& pos, const Color& color, float fontSize,
		Font::Type fontType)
{
	Font::instance().drawText(str, pos, color, fontSize, fontType);
}
void Graphics2D::drawText(const char* str, const Vector2& pos, Texture& tex, uint const color, Font::Type type, float const size)
{
	Font::instance().drawText(str, pos, tex, color, type, size);
}


void Graphics2D::drawTexture(const Texture& texture, const Vector2& pos, const Vector2& size, const Color& color,
					bool fixTexCoord)
{
	drawTexture(texture, pos, size, color, Vector2(0.f, 0.f), Vector2(1.f, 1.f), fixTexCoord);
}

void Graphics2D::drawTexture(const Texture& texture, const Vector2& pos, const Vector2& size, const Color& color,
					const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord)
{
	uvArray[0] = texcoord0.x;	uvArray[1] = texcoord0.y;
	uvArray[2] = texcoord0.x;	uvArray[3] = texcoord1.y;
	uvArray[4] = texcoord1.x;	uvArray[5] = texcoord0.y;
	uvArray[6] = texcoord1.x;	uvArray[7] = texcoord1.y;
	if (fixTexCoord && texture.isValid()) {
		float widthScale = (float)texture.orgWidth() / texture.width();
		float heightScale = (float)texture.orgHeight() / texture.height();
		uvArray[0] *= widthScale;	uvArray[1] *= heightScale;
		uvArray[2] *= widthScale;	uvArray[3] *= heightScale;
		uvArray[4] *= widthScale;	uvArray[5] *= heightScale;
		uvArray[6] *= widthScale;	uvArray[7] *= heightScale;
	}

	device_.setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device_.setTexCoordPointer(2, GL_FLOAT, 0, uvArray);
	device_.setVertexState(true, false, true, false);
	device_.setTexture2D(true, texture.glTexture());
	device_.setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device_.setColor(color);
	Matrix mt, ms;
	mt.translation(Vector3(pos.x, pos.y, 0.f));
	ms.scaling(Vector3(size.x, -size.y, 1.f));
	Matrix m = ms * mt;
	device_.setModelMatrix(m);
	device_.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Graphics2D::drawTextureRotate(const Texture& texture, const Vector2& center, const Vector2& size, const Color& color,
					float angle, bool fixTexCoord)
{
	drawTextureRotate(texture, center, size, color, angle, Vector2(0.f, 0.f), Vector2(1.f, 1.f), fixTexCoord);
}

void Graphics2D::drawTextureRotate(const Texture& texture, const Vector2& center, const Vector2& size, const Color& color,
					float angle, const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord)
{
	uvArray[0] = texcoord0.x;	uvArray[1] = texcoord0.y;
	uvArray[2] = texcoord0.x;	uvArray[3] = texcoord1.y;
	uvArray[4] = texcoord1.x;	uvArray[5] = texcoord0.y;
	uvArray[6] = texcoord1.x;	uvArray[7] = texcoord1.y;
	if (fixTexCoord && texture.isValid()) {
		float widthScale = (float)texture.orgWidth() / texture.width();
		float heightScale = (float)texture.orgHeight() / texture.height();
		uvArray[0] *= widthScale;	uvArray[1] = (uvArray[1]) * heightScale;
		uvArray[2] *= widthScale;	uvArray[3] = (uvArray[3]) * heightScale;
		uvArray[4] *= widthScale;	uvArray[5] = (uvArray[5]) * heightScale;
		uvArray[6] *= widthScale;	uvArray[7] = (uvArray[7]) * heightScale;
	}

	device_.setVertexPointer(2, GL_FLOAT, 0, rotVertices);
	device_.setTexCoordPointer(2, GL_FLOAT, 0, uvArray);
	device_.setVertexState(true, false, true, false);
	device_.setTexture2D(true, texture.glTexture());
	device_.setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device_.setColor(color);
	Matrix mt, ms, mr;
	mt.translation(Vector3(center.x, center.y, 0.f));
	mr.rotationZ(angle);
	ms.scaling(Vector3(size.x, -size.y, 1.f));
	Matrix m = ms * mr * mt;
	device_.setModelMatrix(m);
	device_.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Graphics2D::drawTexture9Grid(const Texture& texture, const Vector2& pos, const Vector2& size, const Color& color,
					const Vector2& texcoord0, const Vector2& texcoord1,
					const Vector2& borderSize, const Vector2& borderCoord)
{
	kuto::Vector2 texcoordIn0 = texcoord0 + borderCoord;
	kuto::Vector2 texcoordIn1 = texcoord1 - borderCoord;
	drawTexture(texture, pos, borderSize, color, texcoord0, texcoordIn0);		// LeftTop
	kuto::Vector2 tempPos(pos.x, pos.y + borderSize.y);
	kuto::Vector2 tempSize(borderSize.x, size.y - borderSize.y * 2.f);
	kuto::Vector2 tempTexcoord0(texcoord0.x, texcoordIn0.y);
	kuto::Vector2 tempTexcoord1(texcoordIn0.x, texcoordIn1.y);
	drawTexture(texture, tempPos, tempSize, color, tempTexcoord0, tempTexcoord1);		// Left
	tempPos.set(pos.x, pos.y + size.y - borderSize.y);
	tempTexcoord0.set(texcoord0.x, texcoordIn1.y);
	tempTexcoord1.set(texcoordIn0.x, texcoord1.y);
	drawTexture(texture, tempPos, borderSize, color, tempTexcoord0, tempTexcoord1);		// LeftBottom
	tempPos.set(pos.x + borderSize.x, pos.y + size.y - borderSize.y);
	tempSize.set(size.x - borderSize.x * 2.f, borderSize.y);
	tempTexcoord0.set(texcoordIn0.x, texcoordIn1.y);
	tempTexcoord1.set(texcoordIn1.x, texcoord1.y);
	drawTexture(texture, tempPos, tempSize, color, tempTexcoord0, tempTexcoord1);		// Bottom
	tempPos.set(pos.x + size.x - borderSize.x, pos.y + size.y - borderSize.y);
	drawTexture(texture, tempPos, borderSize, color, texcoordIn1, texcoord1);		// RightBottom
	tempPos.set(pos.x + size.x - borderSize.x, pos.y + borderSize.y);
	tempSize.set(borderSize.x, size.y - borderSize.y * 2.f);
	tempTexcoord0.set(texcoordIn1.x, texcoordIn0.y);
	tempTexcoord1.set(texcoord1.x, texcoordIn1.y);
	drawTexture(texture, tempPos, tempSize, color, tempTexcoord0, tempTexcoord1);		// Right
	tempPos.set(pos.x + size.x - borderSize.x, pos.y);
	tempTexcoord0.set(texcoordIn1.x, texcoord0.y);
	tempTexcoord1.set(texcoord1.x, texcoordIn0.y);
	drawTexture(texture, tempPos, borderSize, color, tempTexcoord0, tempTexcoord1);		// RightTop
	tempPos.set(pos.x + borderSize.x, pos.y);
	tempSize.set(size.x - borderSize.x * 2.f, borderSize.y);
	tempTexcoord0.set(texcoordIn0.x, texcoord0.y);
	tempTexcoord1.set(texcoordIn1.x, texcoordIn0.y);
	drawTexture(texture, tempPos, tempSize, color, tempTexcoord0, tempTexcoord1);		// Top
}

void Graphics2D::drawRectangle(const Vector2& pos, const Vector2& size, const Color& color)
{
	device_.setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device_.setVertexState(true, false, false, false);
	device_.setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device_.setColor(color);
	Matrix mt, ms;
	mt.translation(Vector3(pos.x, pos.y, 0.f));
	ms.scaling(Vector3(size.x, -size.y, 1.f));
	Matrix m = ms * mt;
	device_.setModelMatrix(m);
	device_.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Graphics2D::fillRectangle(const Vector2& pos, const Vector2& size, const Color& color)
{
	device_.setTexture2D(false, 0);
	device_.setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device_.setVertexState(true, false, false, false);
	device_.setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device_.setColor(color);
	Matrix mt, ms;
	mt.translation(Vector3(pos.x, pos.y, 0.f));
	ms.scaling(Vector3(size.x, -size.y, 1.f));
	Matrix m = ms * mt;
	device_.setModelMatrix(m);
	device_.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Graphics2D::fillRectangleMask(const Vector2& pos, const Vector2& size, const Color& color, const Texture& texture, bool fixTexCoord)
{
	fillRectangleMask(pos, size, color, texture, Vector2(0.f, 0.f), Vector2(1.f, 1.f), fixTexCoord);
}

void Graphics2D::fillRectangleMask(const Vector2& pos, const Vector2& size, const Color& color, const Texture& texture,
					const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord)
{
	uvArray[0] = texcoord0.x;	uvArray[1] = texcoord0.y;
	uvArray[2] = texcoord0.x;	uvArray[3] = texcoord1.y;
	uvArray[4] = texcoord1.x;	uvArray[5] = texcoord0.y;
	uvArray[6] = texcoord1.x;	uvArray[7] = texcoord1.y;
	if (fixTexCoord && texture.isValid()) {
		float widthScale = (float)texture.orgWidth() / texture.width();
		float heightScale = (float)texture.orgHeight() / texture.height();
		uvArray[0] *= widthScale;	uvArray[1] = (uvArray[1]) * heightScale;
		uvArray[2] *= widthScale;	uvArray[3] = (uvArray[3]) * heightScale;
		uvArray[4] *= widthScale;	uvArray[5] = (uvArray[5]) * heightScale;
		uvArray[6] *= widthScale;	uvArray[7] = (uvArray[7]) * heightScale;
	}

	device_.setTexture2D(true, texture.glTexture());
	device_.setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device_.setTexCoordPointer(2, GL_FLOAT, 0, uvArray);
	device_.setVertexState(true, false, true, false);
	device_.setColor(color);
	Matrix mt, ms;
	mt.translation(Vector3(pos.x, pos.y, 0.f));
	ms.scaling(Vector3(size.x, -size.y, 1.f));
	Matrix m = ms * mt;
	device_.setModelMatrix(m);

	device_.setBlendState(true, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	device_.drawArrays(GL_TRIANGLE_STRIP, 0, 4);

	device_.setTexture2D(false, 0);
	device_.setVertexState(true, false, false, false);
	device_.setBlendState(true, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	device_.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

}	// namespace kuto

bool RPG2kUtil::LoadImage(kuto::Texture& texture, const std::string& filename, bool useAlphaPalette, int hue)
{
	static char const* EXT[] = { ".png", ".bmp", ".xyz", };
// search current directory
	for(uint i = 0; i < sizeof(EXT) / sizeof(EXT[0]); i++) {
		std::string temp = filename + EXT[i];
		if (kuto::File::exists(temp.c_str())) {
			texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
			return true;
		}
	}
// search runtime package directory
	std::string dirName = kuto::File::filename(kuto::File::directoryName(filename));
#if RPG2K_IS_WINDOWS
	std::string rtpPath = "D:/ASCII/RPG2000/RTP/";
#elif (RPG2K_IS_MAC_OS_X || RPG2K_IS_IPHONE)
	std::string rtpPath = "/User/Media/Photos/RPG2000/RTP/";
#else
	std::string rtpPath = "./RTP/";
#endif
	rtpPath.append(dirName).append("/").append( kuto::File::filename(filename) );
	for(uint i = 0; i < sizeof(EXT) / sizeof(EXT[0]); i++) {
		std::string temp = rtpPath + EXT[i];
		if (kuto::File::exists(temp.c_str())) {
			texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
			return true;
		}
	}
	return false;
}
