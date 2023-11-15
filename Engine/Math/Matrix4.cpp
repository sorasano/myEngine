/**
* @file Matrix4.cpp
* @brief Matrix4型と基礎的な計算
*/

#include "Matrix4.h"
#include"Vector3.h"
#include <cmath>

//単位行列
Matrix4 Identity() {
	static const Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//拡大縮小行列
Matrix4 Scale(const Vector3& s) {
	Matrix4 result{
		s.x,0.0f,0.0f,0.0f,
		0.0f,s.y,0.0f,0.0f,
		0.0f,0.0f,s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//X軸回りの回転行列
Matrix4 RotateX(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos, sin, 0.0f,
		0.0f,-sin,cos, 0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//Y軸回りの回転行列
Matrix4 RotateY(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos,0.0f,-sin,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		sin,0.0f,cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//Z軸回りの回転行列
Matrix4 RotateZ(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos,sin,0.0f,0.0f,
		-sin,cos,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//平行移動行列
Matrix4 Translate(const Vector3& t) {
	Matrix4 result{
		1.0f,0.0f,0.0f,t.x,
		0.0f,1.0f,0.0f,t.y,
		0.0f,0.0f,1.0f,t.z,
		0.0f,0.0f,0.0f,1.0f

	};

	return result;
}

//座標変換
Vector3 Transform(const Vector3& v, const Matrix4& m) {
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w,
	};

	return result;
}

//代入演算子オーバーロード
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2) {
	Matrix4 result{};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += (m1.m[i][k] * m2.m[k][j]);
			}
		}
	}

	m1 = result;
	return m1;
}


//2項演算子オーバーロード
Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {

	Matrix4 result = m1;

	return result *= m2;
}

Vector3 operator*(const Vector3& v, const Matrix4& m) {
	return Transform(v, m);
}

Vector3 Mul(const Vector3& v, const Matrix4& m) {

	float x, y, z;

	x = (v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]);
	y = (v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]);
	z = (v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]);


	return Vector3(x, y, z);

}

Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}
