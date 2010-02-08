/**
 * @file
 * @brief Color Class
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"


namespace kuto {

class ColorHSV;

/// RGBA Color Class
class Color
{
public:
	Color() {}
	Color(u32 color) { set(color); }
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
	Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
	
	Color operator+(const Color& rhs) const { Color ret(*this); return ret += rhs; }
	Color operator-(const Color& rhs) const { Color ret(*this); return ret -= rhs; }
	Color operator*(const Color& rhs) const { Color ret(*this); return ret *= rhs; }
	Color operator/(const Color& rhs) const { Color ret(*this); return ret /= rhs; }
	Color operator*(float rhs) const { Color ret(*this); return ret *= rhs; }
	Color operator/(float rhs) const { Color ret(*this); return ret /= rhs; }
	
	Color& operator+=(const Color& rhs) { r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; return *this; }
	Color& operator-=(const Color& rhs) { r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a; return *this; }
	Color& operator*=(const Color& rhs) { r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a; return *this; }
	Color& operator/=(const Color& rhs) { r /= rhs.r; g /= rhs.g; b /= rhs.b; a /= rhs.a; return *this; }
	Color& operator*=(float rhs) { r *= rhs; g *= rhs; b *= rhs; a *= rhs; return *this; }
	Color& operator/=(float rhs) { float inv = 1.0f / rhs; r *= inv; g *= inv; b *= inv; a *= inv; return *this; }
	
	friend Color operator-(const Color& rhs) { return Color(-rhs.r, -rhs.g, -rhs.b, -rhs.a); }
	friend Color operator*(float lhs, const Color& rhs) { Color ret(rhs); return ret *= lhs; }
	friend Color operator/(float lhs, const Color& rhs) { Color ret(rhs); return ret /= lhs; }
	
	bool operator==(const Color& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
	bool operator!=(const Color& rhs) const { return !(*this == rhs); }
	
	void set(float r, float g, float b, float a) { this->r = r; this->g = g; this->b = b; this->a = a; }
	void set(u32 color) {
		this->b = (float)(color & 0xFF);
		this->g = (float)((color >> 8) & 0xFF);
		this->r = (float)((color >> 16) & 0xFF);
		this->a = (float)((color >> 24) & 0xFF);
	}
	u32 argb() const {
		u32 ret = ((u32)(a * 255.f) << 24) & 0xFF000000;
		ret |= ((u32)(r * 255.f) << 16) & 0xFF0000;
		ret |= ((u32)(g * 255.f) << 8) & 0xFF00;
		ret |= ((u32)(b * 255.f)) & 0xFF;
		return ret;
	}
	float* pointer() { return reinterpret_cast<float*>(this); }
	const float* pointer() const { return reinterpret_cast<const float*>(this); }
	ColorHSV hsv() const;
	
public:
	float		r;
	float		g;
	float		b;
	float		a;	
};	// class Color


/// HSV Color Class
class ColorHSV
{
public:
	ColorHSV() {}
	ColorHSV(float h, float s, float v) : h(h), s(s), v(v) {}
	ColorHSV(const ColorHSV& color) : h(color.h), s(color.s), v(color.v) {}
	
	void set(float h, float s, float v) { this->h = h; this->s = s; this->v = v; }
	Color rgb() const;

public:
	float		h;
	float		s;
	float		v;	
};	// class ColorHSV

}	// namespace kuto

