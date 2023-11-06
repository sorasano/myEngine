/**
* @file Vector2.h
* @brief Vector2型と基礎的な計算
*/

#pragma once
class Vector2
{
public:
	float x;	//x成分
	float y;	//y成分

public:
	/**
	* コンストラクタ(零ベクトル)
	*/
	Vector2();
	/**
	* コンストラクタ(x成分、y成分を指定して生成)
	*
	* @param[in] x x成分
	* @param[in] y y成分
	*/
	Vector2(float x, float y);

	//メンバ関数
	/**
	* ノルム(長さ)を求める
	*
	* @return float ノルム(長さ)
	*/
	float length() const;
	/**
	* 正規化する
	*
	* @return Vector2 正規化したもの
	*/
	Vector2& normalize();
	/**
	* 内積を求める
	*
	* @param[in] v 元のVector2
	* @return float 内積
	*/
	float dot(const Vector2& v)const;
	/**
	* 外積を求める
	*
	* @param[in] v 元のVector2
	* @return float 外積
	*/
	float cross(const Vector2& v)const;

	// 単項演算子オーバーロード
	Vector2 operator+() const;
	Vector2 operator-() const;

	// 代入演算子オーバーロード
	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(float s);
	Vector2& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応するため、以下のように準備している
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator*(float s, const Vector2& v);
const Vector2 operator/(const Vector2& v, float s);

//補間関数

/**
* 線形補間(1次関数補間)
*
* @param[in] start 始点
* @param[in] end 終点
* @param[in] t 時間
* @return Vector2 補間数値
*/
const Vector2 lerp(Vector2 start, Vector2 end, float t);