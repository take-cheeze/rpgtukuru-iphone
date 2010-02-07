/**
 * @file
 * @brief 4D Vector Class
 * @author takuto
 */
#pragma once

#include <math.h>


namespace kuto {

/// 4D Vector Class
class Vector4
{
public:
	Vector4() {}
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(const Vector4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
	
	Vector4 operator+(const Vector4& rhs) const { Vector4 ret(*this); return ret += rhs; }
	Vector4 operator-(const Vector4& rhs) const { Vector4 ret(*this); return ret -= rhs; }
	Vector4 operator*(const Vector4& rhs) const { Vector4 ret(*this); return ret *= rhs; }
	Vector4 operator/(const Vector4& rhs) const { Vector4 ret(*this); return ret /= rhs; }
	Vector4 operator*(float rhs) const { Vector4 ret(*this); return ret *= rhs; }
	Vector4 operator/(float rhs) const { Vector4 ret(*this); return ret /= rhs; }
	
	Vector4& operator+=(const Vector4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	Vector4& operator-=(const Vector4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
	Vector4& operator*=(const Vector4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
	Vector4& operator/=(const Vector4& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
	Vector4& operator*=(float rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
	Vector4& operator/=(float rhs) { float inv = 1.0f / rhs; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }
	
	friend Vector4 operator-(const Vector4& rhs) { return Vector4(-rhs.x, -rhs.y, -rhs.z, -rhs.w); }
	friend Vector4 operator*(float lhs, const Vector4& rhs) { Vector4 ret(rhs); return ret *= lhs; }
	friend Vector4 operator/(float lhs, const Vector4& rhs) { Vector4 ret(rhs); return ret /= lhs; }
	
	bool operator==(const Vector4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
	bool operator!=(const Vector4& rhs) const { return !(*this == rhs); }
	
	void set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	void normalize() { *this /= length(); }
	Vector4 normalized() const { Vector4 ret(*this); ret.normalize(); return ret; }
	float length() const { return sqrtf(x*x + y*y + z*z + w*w); }
	float lengthSq() const { return x*x + y*y + z*z + w*w; }
	float dot(const Vector4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }
	
	float* pointer() { return reinterpret_cast<float*>(this); }
	const float* pointer() const { return reinterpret_cast<const float*>(this); }
	float operator[](int index) const { return pointer()[index]; }

public:
	float x;
	float y;
	float z;
	float w;
};	// class Vector4
	
}	// namespace kuto

