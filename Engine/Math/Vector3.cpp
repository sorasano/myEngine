/**
* @file Vector3.cpp
* @brief Vector3型と基礎的な計算
*/

#include "Vector3.h"
#include<cmath>

Vector3::Vector3():x(0),y(0),z(0)
{
}

Vector3::Vector3(float x, float y, float z) :x(x), y(y), z(z)
{
}

float Vector3::length() const
{
	return sqrt((x * x) + (y * y) + (z * z));
}

Vector3& Vector3::normalize()
{
	float len = length();
	if (len != 0) {
		return *this /= len;
	}
	return *this;
}

float Vector3::dot(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::cross(const Vector3& v) const
{
	return
	{ y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x };
}

void Vector3::zero()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3 Vector3::operator+() const
{
	return*this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return*this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return*this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return*this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return*this;
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	return v * s;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}

//線形補間
const Vector3 lerp(Vector3 start, Vector3 end, float t) {
	
	Vector3 result;
	
	result.x = (start.x * (1.0f - t)) + (end.x * t);
	result.y = (start.y * (1.0f - t)) + (end.y * t);
	result.z = (start.z * (1.0f - t)) + (end.z * t);

	return result;
}

Vector3 XMMATRIXTransform(const Vector3& v, const XMMATRIX& m)
{
	float w = v.x * m.r[0].m128_f32[3] + v.y * m.r[1].m128_f32[3] + v.z * m.r[2].m128_f32[3] + m.r[3].m128_f32[3];

	Vector3 result{
		(v.x * m.r[0].m128_f32[0] + v.y * m.r[1].m128_f32[0] + v.z * m.r[2].m128_f32[0] + m.r[3].m128_f32[0]) / w,
		(v.x * m.r[0].m128_f32[1] + v.y * m.r[1].m128_f32[1] + v.z * m.r[2].m128_f32[1] + m.r[3].m128_f32[1]) / w,
		(v.x * m.r[0].m128_f32[2] + v.y * m.r[1].m128_f32[2] + v.z * m.r[2].m128_f32[2] + m.r[3].m128_f32[2]) / w,
	};

	return result;
}
