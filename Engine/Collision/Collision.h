/**
* @file Collision.h
* @brief 当たり判定の処理
*/

#pragma once
#include <DirectXMath.h>
#include "wrl.h"
#include "Sprite.h"

enum CollType {
	SQUARE,//矩形
	SPHERE//球
};

struct CollisionData {
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 size = {};
	float radius = 0.0f;
};

class Collision
{
private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:

	/**
	* コンストラクタ
	*/
	Collision() = default;
	/**
	* デストラクタ
	*/
	~Collision() = default;


public:
	/**
	* 矩形同士の当たり判定
	*
	* @param[in] A 1つめの矩形
	* @param[in] B 2つめの矩形
	* @return bool 当たっていたらtrue
	*/
	static bool CheckSquareToSquare(const CollisionData& A, const CollisionData& B);
	/**
	* 球同士の当たり判定
	*
	* @param[in] A 1つめの球
	* @param[in] B 2つめの球
	* @return bool 当たっていたらtrue
	*/
	static bool CheckSphereToSphere(const CollisionData& A, const CollisionData& B);

	/**
	* スプライトとの当たり判定(座標がスプライトの中心点の場合)
	*
	* @param[in] A スプライト
	* @param[in] B 2d座標
	* @return bool 当たっていたらtrue
	*/
	static bool CheckSpriteTo2Dpos(const Sprite* sprite, const XMFLOAT2& pos);

};

