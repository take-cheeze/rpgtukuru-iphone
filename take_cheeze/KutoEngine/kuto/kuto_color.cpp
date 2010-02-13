/**
 * @file
 * @brief Color Class
 * @author project.kuto
 */

#include <cmath>
#include "kuto_color.h"
#include "kuto_utility.h"


namespace kuto {

/**
 * RGBからHSVに変換
 * @return	hsvカラークラス
 */
ColorHSV Color::hsv() const
{
	ColorHSV ret;
	float maxValue = max(r, max(g, b));
	float minValue = min(r, min(g, b));

	ret.v = maxValue;

	if (maxValue == 0.f || maxValue == minValue) {
		ret.s = 0.f;
		ret.h = 0.f;
	} else {
		ret.s = (maxValue - minValue) / maxValue;
		if (maxValue == r) {
			ret.h = (g - b) / (maxValue - minValue) * 60.f + 0.f;
		} else if (maxValue == g) {
			ret.h = (b - r) / (maxValue - minValue) * 60.f + 120.f;
		} else {
			ret.h = (r - g) / (maxValue - minValue) * 60.f + 240.f;
		}
		if (ret.h < 0)
			ret.h += 360;
	}
	return ret;
}

/**
 * HSVからRGBに変換
 * @return	RGBカラー
 */
Color ColorHSV::rgb() const
{
	Color ret;
	ret.a = 1.f;
	
	if (s == 0.f) {
		ret.r = ret.g = ret.b = v;
	} else {
		int hi = ((int)floor(h / 60.f)) % 6;
		float f = (h / 60.f) - (float)hi;
		float p = v * (1.f - s);
		float q = v * (1.f - f * s);
		float t = v * (1.f - (1.f - f) * s);
		switch (hi) {
		case 0:
			ret.r = v;
			ret.g = t;
			ret.b = p;
			break;
		case 1:
			ret.r = q;
			ret.g = v;
			ret.b = p;
			break;
		case 2:
			ret.r = p;
			ret.g = v;
			ret.b = t;
			break;
		case 3:
			ret.r = p;
			ret.g = q;
			ret.b = v;
			break;
		case 4:
			ret.r = t;
			ret.g = p;
			ret.b = v;
			break;
		case 5:
			ret.r = v;
			ret.g = p;
			ret.b = q;
			break;
		}
	}
	
	return ret;
}

}	// namespace kuto
