/**
 * @file
 * @brief 2D Point Class
 * @author project.kuto
 */
#pragma once

#include <cmath>


namespace kuto {

/// 2D Point Class
class Point2
{
public:
	Point2() {}
	Point2(int x, int y) : x(x), y(y) {}
	Point2(const Point2& rhs) : x(rhs.x), y(rhs.y) {}

	Point2 operator+(const Point2& rhs) const { Point2 ret(*this); return ret += rhs; }
	Point2 operator-(const Point2& rhs) const { Point2 ret(*this); return ret -= rhs; }
	Point2 operator*(const Point2& rhs) const { Point2 ret(*this); return ret *= rhs; }
	Point2 operator/(const Point2& rhs) const { Point2 ret(*this); return ret /= rhs; }
	Point2 operator%(const Point2& rhs) const { Point2 ret(*this); return ret %= rhs; }
	Point2 operator*(int rhs) const { Point2 ret(*this); return ret *= rhs; }
	Point2 operator/(int rhs) const { Point2 ret(*this); return ret /= rhs; }

	Point2& operator+=(const Point2& rhs) { x += rhs.x; y += rhs.y; return *this; }
	Point2& operator-=(const Point2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	Point2& operator*=(const Point2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
	Point2& operator/=(const Point2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
	Point2& operator%=(const Point2& rhs) { x %= rhs.x; y %= rhs.y; return *this; }
	Point2& operator*=(int rhs) { x *= rhs; y *= rhs; return *this; }
	Point2& operator/=(int rhs) { x /= rhs; y /= rhs; return *this; }

	friend Point2 operator-(const Point2& rhs) { return Point2(-rhs.x, -rhs.y); }
	friend Point2 operator*(int lhs, const Point2& rhs) { Point2 ret(rhs); return ret *= lhs; }
	friend Point2 operator/(int lhs, const Point2& rhs) { Point2 ret(rhs); return ret /= lhs; }

	bool operator==(const Point2& rhs) const { return x == rhs.x && y == rhs.y; }
	bool operator!=(const Point2& rhs) const { return !(*this == rhs); }

	void set(int x, int y) { this->x = x; this->y = y; }

	int* pointer() { return reinterpret_cast<int*>(this); }
	const int* pointer() const { return reinterpret_cast<const int*>(this); }
	int operator[](int index) const { return pointer()[index]; }
	int length() const { return std::sqrt(x*x + y*y); }

public:
	int x;
	int y;
};	// class Point2

}	// namespace kuto
