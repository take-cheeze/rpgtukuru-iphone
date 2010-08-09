/**
 * @file
 * @brief Font Library
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_singleton.h"


namespace kuto {

/// Fontクラス
class Font : public Singleton<Font>
{
	friend class Singleton<Font>;
public:
	/// Fontタイプ
	enum Type {
		NORMAL = 0,		///< ノーマルフォント
		GOTHIC = 0, MINCHO = 1,
		TYPE_END = 2,
	};

protected:
	Font();
	~Font();

public:
	void clearBuffer();
	void renderBuffer();

	void drawText(const char* str, const Vector2& pos, const Color& color, float size, Font::Type type);
	kuto::Vector2 getTextSize(const char* str, float size, Font::Type type);
};

}	// namespace kuto
