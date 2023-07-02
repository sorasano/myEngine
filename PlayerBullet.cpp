#include "PlayerBullet.h"

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

void PlayerBullet::Initialize(FbxModel* model, XMFLOAT3 position, float speed)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	playerBulletObject = new FbxObject3D;
	playerBulletObject->Initialize();
	playerBulletObject->SetModel(model);

	//���W�A�X�s�[�h���Z�b�g
	this->position_ = position;
	this->playerSpeed = speed;
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
	position_.z += (speed + playerSpeed);
}

void PlayerBullet::OnCollision()
{
	isDead_ = true;
}
