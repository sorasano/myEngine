/**
* @file EnemyBullet.h
* @brief 敵の弾の処理
*/

#include "BaseBullet.h"

#pragma once
class EnemyBullet : public BaseBullet
{
public:

	EnemyBullet() : BaseBullet() {};

	/**
	* 初期化
	*
	* @param[in] model fbxモデル
	* @param[in] position 発生座標
	* @param[in] velocity 発射ベクトル
	* @param[in] playerSpeed プレイヤーのスピード
	*/
	void Initialize(FbxModel* model, const XMFLOAT3& position, const Vector3& velocity, float playerSpeed);
};

