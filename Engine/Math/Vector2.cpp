/**
* @file Vector2.cpp
* @brief Vector2型と基礎的な計算
*/

#include "Vector2.h"
#include<cmath>

Vector2::Vector2() :x(0), y(0)
{
}

Vector2::Vector2(float x, float y) :x(x), y(y)
{
}

float Vector2::length() const
{
	return sqrt((x * x) + (y * y));
}

Vector2& Vector2::normalize()
{
	float len = length();
	if (len != 0) {
		return *this /= len;
	}
	return *this;
}

float Vector2::dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

float Vector2::cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}

Vector2 Vector2::operator+() const
{
	return*this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
	return*this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
	return*this;
}

Vector2& Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
	return*this;
}

Vector2& Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return*this;
}

const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp *= s;
}

const Vector2 operator*(float s, const Vector2& v)
{
	return v * s;
}

const Vector2 operator/(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp /= s;
}

//線形補間
const Vector2 lerp(Vector2 start, Vector2 end, float t) {

	Vector2 result;

	result.x = (start.x * (1.0f - t)) + (end.x * t);
	result.y = (start.y * (1.0f - t)) + (end.y * t);

	return result;
}
