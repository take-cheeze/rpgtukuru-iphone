/**
 * @file
 * @brief 2D Graphics
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_font.h"
#include "kuto_texture.h"


namespace kuto {

class Graphics2D
{
public:
	Graphics2D();
	~Graphics2D();

public:
	void drawText(const char* str, const Vector2& pos, const Color& color, float fontSize,
		Font::Type fontType);
	void drawTexture(const Texture& texture, const Vector2& pos, const Vector2& size, const Color& color,
			bool fixTexCoord = false);
	void drawTexture(const Texture& texture, const Vector2& pos, const Vector2& size, const Color& color,
			const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord = false);
	void drawTextureRotate(const Texture& texture, const Vector2& center, const Vector2& size, const Color& color,
			float angle, bool fixTexCoord = false);
	void drawTextureRotate(const Texture& texture, const Vector2& center, const Vector2& size, const Color& color,
			float angle, const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord = false);
	void drawTexture9Grid(const Texture& texture, const Vector2& pos, const Vector2& size, const Color& color,
			const Vector2& texcoord0, const Vector2& texcoord1, const Vector2& borderSize, const Vector2& borderCoord);
	void drawRectangle(const Vector2& pos, const Vector2& size, const Color& color);
	void fillRectangle(const Vector2& pos, const Vector2& size, const Color& color);
	void fillRectangleMask(const Vector2& pos, const Vector2& size, const Color& color, const Texture& texture,
			bool fixTexCoord = false);
	void fillRectangleMask(const Vector2& pos, const Vector2& size, const Color& color, const Texture& texture,
			const Vector2& texcoord0, const Vector2& texcoord1, bool fixTexCoord = false);
};

}	// namespace kuto

namespace CRpgUtil
{
	bool LoadImage(kuto::Texture& texture, const std::string& filename, bool useAlphaPalette, int hue = 0);
} // namespace CRpgUtil
