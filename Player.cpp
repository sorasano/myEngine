#include "Player.h"
#define PI 3.1415

Player* Player::GetInstance()
{
	static Player instance;
	return &instance;
}

Player::Player()
{
}

Player::~Player()
{
	FBX_SAFE_DELETE(playerObject);
}

void Player::Initialize(Input* input, FbxModel* playerModel, FbxModel* playerBulletModel, FbxModel* fRModel, FbxModel* bRModel)
{
	//���͂̃Z�b�g
	this->input_ = input;

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	playerObject = new FbxObject3D;
	playerObject->Initialize();
	playerObject->SetModel(playerModel);

	//�e���f���Z�b�g
	this->playerBulletModel_ = playerBulletModel;

	//���e�B�N�����f���Z�b�g
	frontReticleObject = new FbxObject3D;
	frontReticleObject->Initialize();
	frontReticleObject->SetModel(fRModel);

	backReticleObject = new FbxObject3D;
	backReticleObject->Initialize();
	backReticleObject->SetModel(bRModel);

	//���W�A���ϊ�
	fRRotation_.y = static_cast<float>(90 * (PI / 180));
	bRRotation_.y = static_cast<float>(90 * (PI / 180));

}

void Player::Update()
{
	//�ړ�
	Move();

	//�e�̍X�V
	BulletUpdate();

	if (isInvincible) {
		invincibleTimer++;
		if (invincibleTimer > InvincibleTime) {
			isInvincible = false;
			invincibleTimer = 0;
		}
	}

	//���e�B�N���̍X�V
	UpdateRaticle();

	playerObject->SetPosition(position_);
	playerObject->SetScale(scale_);
	playerObject->SetRotate(rotation_);
	playerObject->Update();
}


void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (isInvincible) {

	}
	else {
		playerObject->Draw(cmdList);
	}

	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullet_)
	{
		bullet->Draw(cmdList);
	}

	//���e�B�N��
	frontReticleObject->Draw(cmdList);
	backReticleObject->Draw(cmdList);

}

void Player::SpeedUpByEnemy()
{
	if (addSpeed + addSpeedByEnemy >= MaxSpeed) {
		addSpeed = MaxSpeed;
	}
	else {
		addSpeed += addSpeedByEnemy;
	}
}

void Player::SpeedDownByEnemy()
{
	if (!isInvincible) {
		if (addSpeed - subSpeedByEnemy <= 0) {
			addSpeed = 0;
		}
		else {
			addSpeed -= subSpeedByEnemy;
		}
	}
}

void Player::Move()
{

	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_W)) {
			if (MoveMax.y > position_.y) { position_.y += speedXY; }
		}
		else if (input_->PushKey(DIK_S)) {
			if (-MoveMax.y < position_.y) { position_.y -= speedXY; }
		}

		if (input_->PushKey(DIK_A)) {
			if (-MoveMax.x < position_.x) { position_.x -= speedXY; }
		}
		else if (input_->PushKey(DIK_D)) {
			if (MoveMax.x > position_.x) { position_.x += speedXY; }
		}

	}

	position_.z += speedZ + addSpeed;

}

void Player::Shot()
{
	bulletCoolTimer++;

	if (input_->PushKey(DIK_SPACE)) {

		if (BulletCoolTime < bulletCoolTimer) {
			MakeBullet();

			bulletCoolTimer = 0;
		}
	}
}

void Player::BulletUpdate()
{
	//---�e---
	Shot();
	//�v���C���[�̒e�X�V
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullet_)
	{
		bullet->Update();
	}
	//�f�X�t���O�̗������e���폜
	playerBullet_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {return bullet->GetIsDead(); });
}

void Player::MakeBullet()
{
	//�e�̐���
	std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
	newBullet->Initialize(playerBulletModel_, position_, speedZ+addSpeed);
	playerBullet_.push_back(std::move(newBullet));
}

void Player::UpdateRaticle()
{
	//���@�̃��[���h���W����3D���e�B�N���̃��[���h���W���v�Z
	{
		fRPosition_ = position_;
		fRPosition_.z = fRPosition_.z + kDistancePlayerTo3DFrontReticle;

		bRPosition_ = position_;
		bRPosition_.z = bRPosition_.z + kDistancePlayerTo3DBackReticle;
	}

	frontReticleObject->SetPosition(fRPosition_);
	frontReticleObject->SetRotate(fRRotation_);
	frontReticleObject->SetScale(fRScale_);
	frontReticleObject->Update();

	backReticleObject->SetPosition(bRPosition_);
	backReticleObject->SetRotate(bRRotation_);
	backReticleObject->SetScale(bRScale_);
	backReticleObject->Update();
}

XMFLOAT3 Player::GetBulletPosition(int i)
{
	auto it = playerBullet_.begin();
	std::advance(it,i);

	return it->get()->GetPosition();
}

XMFLOAT3 Player::GetBulletRotation(int i)
{
	auto it = playerBullet_.begin();
	std::advance(it, i);

	return it->get()->GetRotation();
}

XMFLOAT3 Player::GetBulletScale(int i)
{
	auto it = playerBullet_.begin();
	std::advance(it, i);

	return it->get()->GetScale();
}

XMFLOAT3 Player::GetBulletColSize(int i)
{
	auto it = playerBullet_.begin();
	std::advance(it, i);

	return it->get()->GetColSize();
}

void Player::SetBulletIsDead(bool isDead, int i)
{
	auto it = playerBullet_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}


