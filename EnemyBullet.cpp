#include "EnemyBullet.h"
#include <cmath>
#define PI 3.1415

EnemyBullet* EnemyBullet::GetInstance()
{
	static EnemyBullet instance;
	return &instance;
}

EnemyBullet::EnemyBullet()
{
}

EnemyBullet::~EnemyBullet()
{
	FBX_SAFE_DELETE(EnemyBulletObject);
}

void EnemyBullet::Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity)
{

	//3dオブジェクト生成とモデルのセット
	EnemyBulletObject = new FbxObject3D;
	EnemyBulletObject->Initialize();
	EnemyBulletObject->SetModel(model);

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

void EnemyBullet::Update()
{
	Move();

	//一定時間経過で弾削除
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	EnemyBulletObject->SetPosition(position_);
	EnemyBulletObject->SetScale(scale_);
	EnemyBulletObject->SetRotate(rotation_);
	EnemyBulletObject->Update();
}

void EnemyBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	EnemyBulletObject->Draw(cmdList);
}

void EnemyBullet::Move()
{
	position_.x += velocity_.x;
	position_.y += velocity_.y;
	position_.z += velocity_.z;
}

void EnemyBullet::OnCollision()
{
	isDead_ = true;
}

CollisionData EnemyBullet::GetColData()
{
	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}