/**
 * @file
 * @brief 3D Vector Class
 * @author takuto
 */
#pragma once

#include <math.h>


namespace kuto {

/// 3D Vector Class
class Vector3
{
public:
	Vector3() {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3(const Vector3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
	
	Vector3 operator+(const Vector3& rhs) const { Vector3 ret(*this); return ret += rhs; }
	Vector3 operator-(const Vector3& rhs) const { Vector3 ret(*this); return ret -= rhs; }
	Vector3 operator*(const Vector3& rhs) const { Vector3 ret(*this); return ret *= rhs; }
	Vector3 operator/(const Vector3& rhs) const { Vector3 ret(*this); return ret /= rhs; }
	Vector3 operator*(float rhs) const { Vector3 ret(*this); return ret *= rhs; }
	Vector3 operator/(float rhs) const { Vector3 ret(*this); return ret /= rhs; }
	
	Vector3& operator+=(const Vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3& operator-=(const Vector3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3& operator*=(const Vector3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	Vector3& operator/=(const Vector3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
	Vector3& operator*=(float rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
	Vector3& operator/=(float rhs) { float inv = 1.0f / rhs; x *= inv; y *= inv; z *= inv; return *this; }
	
	friend Vector3 operator-(const Vector3& rhs) { return Vector3(-rhs.x, -rhs.y, -rhs.z); }
	friend Vector3 operator*(float lhs, const Vector3& rhs) { Vector3 ret(rhs); return ret *= lhs; }
	friend Vector3 operator/(float lhs, const Vector3& rhs) { Vector3 ret(rhs); return ret /= lhs; }

	bool operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
	bool operator!=(const Vector3& rhs) const { return !(*this == rhs); }
	
	void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	void normalize() { *this /= length(); }
	Vector3 normalized() const { Vector3 ret(*this); ret.normalize(); return ret; }
	float length() const { return sqrtf(x*x + y*y + z*z); }
	float lengthSq() const { return x*x + y*y + z*z; }
	float dot(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
	Vector3 cross(const Vector3& rhs) const { return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x); }
	
	float* pointer() { return reinterpret_cast<float*>(this); }
	const float* pointer() const { return reinterpret_cast<const float*>(this); }
	float operator[](int index) const { return pointer()[index]; }

public:
	float x;
	float y;
	float z;
};	// class Vector3
	
}	// namespace kuto

