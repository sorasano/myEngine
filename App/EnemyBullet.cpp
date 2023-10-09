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
	FBX_SAFE_DELETE(EnemyBulletObject_);
}

void EnemyBullet::Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity,float playerSpeed)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	EnemyBulletObject_ = new FbxObject3D;
	EnemyBulletObject_->Initialize();
	EnemyBulletObject_->SetModel(model);

	//���W�A�X�s�[�h���Z�b�g
	this->position_ = position;
	this->velocity_ = velocity;

	//scale_.z = 5.0f;

	//�p�x���x�N�g�����狁�߂ă��f���ɔ��f
	//Y������p�x
	this->rotation_.y = std::atan2(velocity_.x, velocity_.z - playerSpeed);

	//���������̒��������߂�
	Vector3 velocityXZ = { velocity_.x,0.0f,velocity_.z - playerSpeed};
	float length = velocityXZ.length();

	//X������p�x
	this->rotation_.x = std::atan2(-velocity_.y, length);

}

void EnemyBullet::Update()
{
	Move();

	//��莞�Ԍo�߂Œe�폜
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	EnemyBulletObject_->SetPosition(position_);
	EnemyBulletObject_->SetScale(scale_);
	EnemyBulletObject_->SetRotate(rotation_);
	EnemyBulletObject_->Update();
}

void EnemyBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	EnemyBulletObject_->Draw(cmdList);
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

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}