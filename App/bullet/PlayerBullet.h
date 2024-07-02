/**
* @file PlayerBullet.cpp
* @brief プレイヤーの弾の処理
*/

#include "BaseBullet.h"

#pragma once
class PlayerBullet : public BaseBullet
{
public:

	PlayerBullet() : BaseBullet() {};

	/**
	* 初期化
	*
	* @param[in] model fbxモデル
	* @param[in] position 発生座標
	* @param[in] velocity 発射ベクトル
	*/
	void Initialize(FbxModel* model, const XMFLOAT3& position, const Vector3& velocity);

};

