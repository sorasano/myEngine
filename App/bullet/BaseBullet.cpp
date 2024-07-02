/**
* @file BaseBullet.cpp
* @brief 弾の基本機能
*/

#include "BaseBullet.h"

BaseBullet::BaseBullet()
{
}

void BaseBullet::Update()
{
	//一定時間経過で弾削除
	if (--deathTimer_ <= 0) {
	isDead_ = true;
	}

	Move();

	bulletObject_->SetPosition(position_);
	bulletObject_->SetScale(scale_);
	bulletObject_->SetRotate(rotation_);
	bulletObject_->Update();
}

void BaseBullet::EnemyUpdate(float cameraZ)
{

	float deletePos = cameraZ + 5;

	//一定位置で削除
	if (deletePos >= position_.z) {
		isDead_ = true;
	}

	Update();
}

void BaseBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	bulletObject_->Draw(cmdList);
}

void BaseBullet::Move()
{
	VecAddXMFLOAT3(position_, velocity_);
}

void BaseBullet::OnCollision()
{
	isDead_ = true;
}

CollisionData BaseBullet::GetColData() const
{
	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}
