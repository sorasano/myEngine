/**
* @file Matrix4.h
* @brief Matrix4型と基礎的な計算
*/

#pragma once
#include"Vector3.h"

/// <summary>
/// 行列
/// </summary>
class Matrix4 {
  public:
	// 行x列
	float m[4][4];

	// コンストラクタ
	Matrix4() = default;
	// 成分を指定しての生成
	Matrix4(
	  float m00, float m01, float m02, float m03,
	  float m10, float m11, float m12, float m13,
	  float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

};

//単位行列
Matrix4 Identity();

//拡大縮小行列
Matrix4 Scale(const Vector3& s);

//回転行列
Matrix4 RotateX(float angle);
Matrix4 RotateY(float angle);
Matrix4 RotateZ(float angle);

//平行移動行列
Matrix4 Translate(const Vector3& t);

//座標変換
Vector3 Transform(const Vector3& v, const Matrix4& m);

//代入演算子オーバーロード
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

//2項演算子オーバーロード
Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);

Vector3 operator*(const Vector3& v, const Matrix4& m);

//行列とベクトルの掛け算
Vector3 Mul(const Vector3& v, const Matrix4& m);
