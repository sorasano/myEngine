#include "BossBullet.h"
#include <cmath>
#define PI 3.1415

BossBullet* BossBullet::GetInstance()
{
	static BossBullet instance;
	return &instance;
}

BossBullet::BossBullet()
{
}

BossBullet::~BossBullet()
{
	FBX_SAFE_DELETE(BossBulletObject);
}

void BossBullet::Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity, float playerSpeed)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	BossBulletObject = new FbxObject3D;
	BossBulletObject->Initialize();
	BossBulletObject->SetModel(model);

	//���W�A�X�s�[�h���Z�b�g
	this->position_ = position;
	this->velocity_ = velocity;

	//scale_.z = 5.0f;

	//�p�x���x�N�g�����狁�߂ă��f���ɔ��f
	//Y������p�x
	this->rotation_.y = std::atan2(velocity_.x, velocity_.z - playerSpeed);

	//���������̒��������߂�
	Vector3 velocityXZ = { velocity_.x,0.0f,velocity_.z - playerSpeed };
	float length = velocityXZ.length();

	//X������p�x
	this->rotation_.x = std::atan2(-velocity_.y, length);

}

void BossBullet::Update()
{
	Move();

	//��莞�Ԍo�߂Œe�폜
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	BossBulletObject->SetPosition(position_);
	BossBulletObject->SetScale(scale_);
	BossBulletObject->SetRotate(rotation_);
	BossBulletObject->Update();
}

void BossBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	BossBulletObject->Draw(cmdList);
}

void BossBullet::Move()
{
	position_.x += velocity_.x;
	position_.y += velocity_.y;
	position_.z += velocity_.z;
}

void BossBullet::OnCollision()
{
	isDead_ = true;
}

CollisionData BossBullet::GetColData()
{
	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}