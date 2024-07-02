/**
* @file PlayerBullet.cpp
* @brief プレイヤーの弾の処理
*/

#include "PlayerBullet.h"
#include <cmath>

void PlayerBullet::Initialize(FbxModel* model, const XMFLOAT3& position, const Vector3& velocity)
{
	//3dオブジェクト生成とモデルのセット
	bulletObject_ = new FbxObject3D;
	bulletObject_->Initialize();
	bulletObject_->SetModel(model);

	//座標、スピードをセット
	this->position_ = position;
	this->velocity_ = velocity;

	//角度をベクトルから求めてモデルに反映
	//Y軸周り角度
	this->rotation_.y = std::atan2(velocity_.x, velocity_.z);

	//横軸方向の長さを求める
	Vector3 velocityXZ = { velocity_.x,0.0f,velocity_.z };
	float length = velocityXZ.length();

	//X軸周り角度
	this->rotation_.x = std::atan2(-velocity_.y, length);

}
