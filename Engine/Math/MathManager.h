/**
* @file MathManager.h
* @brief 数学型のインクルードまとめ
*/

#pragma once

//円周率
#define PI 3.141592653589793238462643383279502884

#include <DirectXMath.h>

#include "Vector3.h"
#include "Vector2.h"

//DirectX::を省略
using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMMATRIX = DirectX::XMMATRIX;

/**
* 逆行列化
*
* @param[in] mat 元行列
*/
XMMATRIX MatrixInverse(const XMMATRIX& mat);