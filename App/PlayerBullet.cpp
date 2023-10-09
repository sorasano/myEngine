#include "PlayerBullet.h"
#include <cmath>
#define PI 3.1415

PlayerBullet* PlayerBullet::GetInstance()
{
	static PlayerBullet instance;
	return &instance;
}

PlayerBullet::PlayerBullet()
{
}

PlayerBullet::~PlayerBullet()
{
	FBX_SAFE_DELETE(playerBulletObject_);
}

void PlayerBullet::Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity)
{

	//3dオブジェクト生成とモデルのセット
	playerBulletObject_ = new FbxObject3D;
	playerBulletObject_->Initialize();
	playerBulletObject_->SetModel(model);

	//座標、スピードをセット
	this->position_ = position;
	this->velocity_ = velocity;

	//scale_.z = 5.0f;

	//角度をベクトルから求めてモデルに反映
	//Y軸周り角度
	this->rotation_.y = std::atan2(velocity_.x, velocity_.z);

	//横軸方向の長さを求める
	Vector3 velocityXZ = { velocity_.x,0.0f,velocity_.z };
	float length = velocityXZ.length();

	//X軸周り角度
	this->rotation_.x = std::atan2(-velocity_.y, length);

}

void PlayerBullet::Update()
{
	Move();

	//一定時間経過で弾削除
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	playerBulletObject_->SetPosition(position_);
	playerBulletObject_->SetScale(scale_);
	playerBulletObject_->SetRotate(rotation_);
	playerBulletObject_->Update();
}

void PlayerBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	playerBulletObject_->Draw(cmdList);
}

void PlayerBullet::Move()
{
	position_.x += velocity_.x;
	position_.y += velocity_.y;
	position_.z += velocity_.z;
}

void PlayerBullet::OnCollision()
{
	isDead_ = true;
}

CollisionData PlayerBullet::GetColData()
{
	CollisionData colData;

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}
