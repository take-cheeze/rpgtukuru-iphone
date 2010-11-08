/**
 * @file
 * @brief 2D Graphics
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_font.h"
#include "kuto_texture.h"

#include <boost/noncopyable.hpp>


namespace kuto {

class GraphicsDevice;

class Graphics2D : boost::noncopyable
{
public:
	Graphics2D(GraphicsDevice& dev);

public:
	void drawText(const char* str, const Vector2& pos
	, const Color& color, float fontSize, Font::Type fontType);
	void drawText(const char* str, const Vector2& pos, Texture& tex
	, uint const color, Font::Type type, float const size = 12.f);

	void drawTexture(const Texture& texture, const Vector2& pos, const Vector2& size
	, const Color& color, bool fixTexCoord = false);
	void drawTexture(const Texture& texture, const Vector2& pos, const Vector2& size
	, const Color& color, const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord = false);

	void drawTextureRotate(const Texture& texture, const Vector2& center, const Vector2& size
	, const Color& color, float angle, bool fixTexCoord = false);
	void drawTextureRotate(const Texture& texture, const Vector2& center, const Vector2& size
	, const Color& color, float angle, const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord = false);
	void drawTexture9Grid(const Texture& texture, const Vector2& pos, const Vector2& size
	, const Color& color, const Vector2& texcoord0, const Vector2& texcoord1, const Vector2& borderSize, const Vector2& borderCoord);

	void drawRectangle(const Vector2& pos, const Vector2& size, const Color& color);
	void fillRectangle(const Vector2& pos, const Vector2& size, const Color& color);
	void fillRectangleMask(const Vector2& pos, const Vector2& size
	, const Color& color, const Texture& texture, bool fixTexCoord = false);
	void fillRectangleMask(const Vector2& pos, const Vector2& size
	, const Color& color, const Texture& texture
	, const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord = false);

private:
	GraphicsDevice& device_;
};

}	// namespace kuto

namespace RPG2kUtil
{
	bool LoadImage(kuto::Texture& texture, const std::string& filename, bool useAlphaPalette, int hue = 0);
} // namespace CRpgUtil
