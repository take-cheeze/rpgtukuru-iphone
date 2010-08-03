/**
 * @file
 * @brief 2D Vector Class
 * @author project.kuto
 */
#pragma once

#include <math.h>


namespace kuto {

/// 2D Vector Class
class Vector2
{
public:
	Vector2() {}
	Vector2(float val) : x(val), y(val) {}
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2(const Vector2& vec) : x(vec.x), y(vec.y) {}

	Vector2 operator+(const Vector2& rhs) const { Vector2 ret(*this); return ret += rhs; }
	Vector2 operator-(const Vector2& rhs) const { Vector2 ret(*this); return ret -= rhs; }
	Vector2 operator*(const Vector2& rhs) const { Vector2 ret(*this); return ret *= rhs; }
	Vector2 operator/(const Vector2& rhs) const { Vector2 ret(*this); return ret /= rhs; }
	Vector2 operator*(float rhs) const { Vector2 ret(*this); return ret *= rhs; }
	Vector2 operator/(float rhs) const { Vector2 ret(*this); return ret /= rhs; }

	Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
	Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	Vector2& operator*=(const Vector2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
	Vector2& operator/=(const Vector2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
	Vector2& operator+=(float rhs) { x += rhs; y += rhs; return *this; }
	Vector2& operator-=(float rhs) { float inv = 1.0f / rhs; x -= inv; y -= inv; return *this; }
	Vector2& operator*=(float rhs) { x *= rhs; y *= rhs; return *this; }
	Vector2& operator/=(float rhs) { float inv = 1.0f / rhs; x *= inv; y *= inv; return *this; }

	friend Vector2 operator-(const Vector2& rhs) { return Vector2(-rhs.x, -rhs.y); }
	friend Vector2 operator*(float lhs, const Vector2& rhs) { Vector2 ret(rhs); return ret *= lhs; }
	friend Vector2 operator/(float lhs, const Vector2& rhs) { Vector2 ret(rhs); return ret /= lhs; }

	bool operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }
	bool operator!=(const Vector2& rhs) const { return !(*this == rhs); }

	void set(float x, float y) { this->x = x; this->y = y; }
	void normalize() { *this /= length(); }
	Vector2 normalized() const { Vector2 ret(*this); ret.normalize(); return ret; }
	float length() const { return sqrtf(x*x + y*y); }
	float lengthSq() const { return x*x + y*y; }
	float dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }

	float* pointer() { return reinterpret_cast<float*>(this); }
	const float* pointer() const { return reinterpret_cast<const float*>(this); }
	float operator[](int index) const { return pointer()[index]; }

public:
	float x;
	float y;
};	// class Vector2

}	// namespace kuto

