#include "PlayerBullet.h"
#include <cmath>

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
	FBX_SAFE_DELETE(playerBulletObject);
}

void PlayerBullet::Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	playerBulletObject = new FbxObject3D;
	playerBulletObject->Initialize();
	playerBulletObject->SetModel(model);

	//���W�A�X�s�[�h���Z�b�g
	this->position_ = position;
	this->velocity_ = velocity;

	//�p�x���x�N�g�����狁�߂ă��f���ɔ��f
	this->rotation_.y = atan2(velocity_.z, velocity_.x);

	Vector3 velocityXZ = { velocity_.x,0.0f,velocity_.z };
	float length = velocityXZ.length();

	this->rotation_.x = atan2(-velocity_.y, length);

	scale_.z = 20.0f;

}

void PlayerBullet::Update()
{
	Move();

	//��莞�Ԍo�߂Œe�폜
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	playerBulletObject->SetPosition(position_);
	playerBulletObject->SetScale(scale_);
	playerBulletObject->SetRotate(rotation_);
	playerBulletObject->Update();
}

void PlayerBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	playerBulletObject->Draw(cmdList);
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
