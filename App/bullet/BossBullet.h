/**
* @file BossBullet.h
* @brief ボスの弾の処理
*/

#include "BaseBullet.h"

#pragma once
class BossBullet : public BaseBullet
{
public:

	BossBullet() : BaseBullet() {};
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
