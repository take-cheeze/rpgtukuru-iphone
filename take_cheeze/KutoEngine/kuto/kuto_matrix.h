/**
 * @file
 * @brief Matrix Class
 * @author project.kuto
 */
#pragma once

#include <cstring>
#include <cmath>
#include <math.h>


namespace kuto {

class Matrix
{
public:
	Matrix() {}
	Matrix(const float* values) { std::memcpy(pointer(), values, sizeof(float) * 16); }
	Matrix(const Matrix& matrix) { std::memcpy(pointer(), matrix.pointer(), sizeof(float) * 16); }

	float* pointer() { return reinterpret_cast<float*>(this); }
	const float* pointer() const { return reinterpret_cast<const float*>(this); }
	Vector4& operator[](int index) { return colmuns[index]; }
	const Vector4& operator[](int index) const { return colmuns[index]; }

	void identity() {
		colmuns[0].set(1.0f, 0.0f, 0.0f, 0.0f);
		colmuns[1].set(0.0f, 1.0f, 0.0f, 0.0f);
		colmuns[2].set(0.0f, 0.0f, 1.0f, 0.0f);
		colmuns[3].set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void ortho(float left, float right, float top, float bottom, float near, float far) {
		float width = right - left;
		float height = top - bottom;
		float depth = far - near;
		colmuns[0].set(2.0f / width, 0.0f, 0.0f, 0.0f);
		colmuns[1].set(0.0f, 2.0F / height, 0.0f, 0.0f);
		colmuns[2].set(0.0f, 0.0f, -(2.0F) / depth, 0.0f);
		colmuns[3].set(-(right + left) / width, -(top + bottom) / height, -(far + near) / depth, 1.0f);
	}
	void translation(const Vector3& trans) {
		colmuns[0].set(1.0f, 0.0f, 0.0f, 0.0f);
		colmuns[1].set(0.0f, 1.0f, 0.0f, 0.0f);
		colmuns[2].set(0.0f, 0.0f, 1.0f, 0.0f);
		colmuns[3].set(trans.x, trans.y, trans.z, 1.0f);
	}
	void scaling(const Vector3& scale) {
		colmuns[0].set(scale.x, 0.0f, 0.0f, 0.0f);
		colmuns[1].set(0.0f, scale.y, 0.0f, 0.0f);
		colmuns[2].set(0.0f, 0.0f, scale.z, 0.0f);
		colmuns[3].set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void rotationX(float angle) {
		float cosX = std::cos(angle);
		float sinX = std::sin(angle);
		colmuns[0].set(1.0f, 0.f, 0.0f, 0.0f);
		colmuns[1].set(0.0f, cosX, sinX, 0.0f);
		colmuns[2].set(0.0f,-sinX, cosX, 0.0f);
		colmuns[3].set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void rotationY(float angle) {
		float cosY = std::cos(angle);
		float sinY = std::sin(angle);
		colmuns[0].set(cosY, 0.0f,-sinY, 0.0f);
		colmuns[1].set(0.0f, 1.0f, 0.0f, 0.0f);
		colmuns[2].set(sinY, 0.0f, cosY, 0.0f);
		colmuns[3].set(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void rotationZ(float angle) {
		float cosZ = std::cos(angle);
		float sinZ = std::sin(angle);
		colmuns[0].set( cosZ, sinZ, 0.0f, 0.0f);
		colmuns[1].set(-sinZ, cosZ, 0.0f, 0.0f);
		colmuns[2].set( 0.0f, 0.0f, 1.0f, 0.0f);
		colmuns[3].set( 0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	Matrix& operator*=(const Matrix& rhs) {
		Matrix ma(*this);
		float* fr = pointer();
		const float* fa = ma.pointer();
		const float* fb = rhs.pointer();
		fr[0] = fa[0] * fb[0] + fa[1] * fb[4] + fa[2] * fb[8]  + fa[3] * fb[12];
		fr[1] = fa[0] * fb[1] + fa[1] * fb[5] + fa[2] * fb[9]  + fa[3] * fb[13];
		fr[2] = fa[0] * fb[2] + fa[1] * fb[6] + fa[2] * fb[10] + fa[3] * fb[14];
		fr[3] = fa[0] * fb[3] + fa[1] * fb[7] + fa[2] * fb[11] + fa[3] * fb[15];
		
		fr[4] = fa[4] * fb[0] + fa[5] * fb[4] + fa[6] * fb[8]  + fa[7] * fb[12];
		fr[5] = fa[4] * fb[1] + fa[5] * fb[5] + fa[6] * fb[9]  + fa[7] * fb[13];
		fr[6] = fa[4] * fb[2] + fa[5] * fb[6] + fa[6] * fb[10] + fa[7] * fb[14];
		fr[7] = fa[4] * fb[3] + fa[5] * fb[7] + fa[6] * fb[11] + fa[7] * fb[15];
		
		fr[8]  = fa[8] * fb[0] + fa[9] * fb[4] + fa[10] * fb[8]  + fa[11] * fb[12];
		fr[9]  = fa[8] * fb[1] + fa[9] * fb[5] + fa[10] * fb[9]  + fa[11] * fb[13];
		fr[10] = fa[8] * fb[2] + fa[9] * fb[6] + fa[10] * fb[10] + fa[11] * fb[14];
		fr[11] = fa[8] * fb[3] + fa[9] * fb[7] + fa[10] * fb[11] + fa[11] * fb[15];
		
		fr[12] = fa[12] * fb[0] + fa[13] * fb[4] + fa[14] * fb[8]  + fa[15] * fb[12];
		fr[13] = fa[12] * fb[1] + fa[13] * fb[5] + fa[14] * fb[9]  + fa[15] * fb[13];
		fr[14] = fa[12] * fb[2] + fa[13] * fb[6] + fa[14] * fb[10] + fa[15] * fb[14];
		fr[15] = fa[12] * fb[3] + fa[13] * fb[7] + fa[14] * fb[11] + fa[15] * fb[15];
		return *this;
	}
	Matrix operator*(const Matrix& rhs) const { Matrix ret(*this); return ret *= rhs; }	
	
private:
	Vector4		colmuns[4];
};	// class Matrix

}	// namespace kuto
