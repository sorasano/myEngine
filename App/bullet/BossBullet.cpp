/**
* @file BossBullet.cpp
* @brief ボスの弾の処理
*/

#include "BossBullet.h"
#include <cmath>

BossBullet::BossBullet()
{
}

BossBullet::~BossBullet()
{
	FBX_SAFE_DELETE(BossBulletObject_);
}

void BossBullet::Initialize(FbxModel* model, const XMFLOAT3& position, const Vector3& velocity, float playerSpeed)
{

	//3dオブジェクト生成とモデルのセット
	BossBulletObject_ = new FbxObject3D;
	BossBulletObject_->Initialize();
	BossBulletObject_->SetModel(model);

	//座標、スピードをセット
	this->position_ = position;
	this->velocity_ = velocity;

	//scale_.z = 5.0f;

	//角度をベクトルから求めてモデルに反映
	//Y軸周り角度
	this->rotation_.y = std::atan2(velocity_.x, velocity_.z - playerSpeed);

	//横軸方向の長さを求める
	Vector3 velocityXZ = { velocity_.x,0.0f,velocity_.z - playerSpeed };
	float length = velocityXZ.length();

	//X軸周り角度
	this->rotation_.x = std::atan2(-velocity_.y, length);

}

void BossBullet::Update()
{
	Move();

	//一定時間経過で弾削除
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	BossBulletObject_->SetPosition(position_);
	BossBulletObject_->SetScale(scale_);
	BossBulletObject_->SetRotate(rotation_);
	BossBulletObject_->Update();
}

void BossBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	BossBulletObject_->Draw(cmdList);
}

void BossBullet::Move()
{
	VecAddXMFLOAT3(position_, velocity_);
}

void BossBullet::OnCollision()
{
	isDead_ = true;
}

CollisionData BossBullet::GetColData() const
{
	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}