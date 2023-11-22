/**
* @file Vector3.h
* @brief Vector3型と基礎的な計算
*/
#include <DirectXMath.h>

using XMMATRIX = DirectX::XMMATRIX;

#pragma once
class Vector3
{
public:
	float x;	//x成分
	float y;	//y成分
	float z;	//z成分

public:
	/**
	* コンストラクタ(零ベクトル)
	*/
	Vector3();
	/**
	* コンストラクタ(x成分、y成分、z成分を指定して生成)
	* 
	* @param[in] x x成分
	* @param[in] y y成分
	* @param[in] z z成分
	*/
	Vector3(float x, float y, float z);

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
	* @return Vector3 正規化したもの
	*/
	Vector3& normalize();
	/**
	* 内積を求める
	*
	* @param[in] v 元のVector3
	* @return float 内積
	*/
	float dot(const Vector3& v)const;
	/**
	* 外積を求める
	*
	* @param[in] v 元のVector3
	* @return Vector3 外積
	*/
	Vector3 cross(const Vector3& v)const;

// 単項演算子オーバーロード
	Vector3 operator+() const;
	Vector3 operator-() const;

	// 代入演算子オーバーロード
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応するため、以下のように準備している
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

//補間関数

/**
* 線形補間(1次関数補間)
*
* @param[in] start 始点
* @param[in] end 終点
* @param[in] t 時間
* @return Vector3 補間数値
*/
const Vector3 lerp(Vector3 start, Vector3 end, float t);

//XMMATRIX

/**
* 合成
*
*/
Vector3 XMMATRIXTransform(const Vector3& v, const XMMATRIX& m);