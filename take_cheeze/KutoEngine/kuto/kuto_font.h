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
public:
	/// Fontタイプ
	enum FONT_TYPE {
		TYPE_NORMAL,		///< ノーマルフォント
	};
	
public:
	Font();
	~Font();

public:
	void clearBuffer();
	void renderBuffer();
	
	void drawText(const char* str, const Vector2& pos, const Color& color, float size, FONT_TYPE type);
	kuto::Vector2 getTextSize(const char* str, float size, FONT_TYPE type);
};
	
}	// namespace kuto

