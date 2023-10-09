#include "Player.h"

#include "Vector3.h"
#define PI 3.1415

#include "Imgui.h"

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
	FBX_SAFE_DELETE(playerObject_);
	FBX_SAFE_DELETE(playerModel_);
	FBX_SAFE_DELETE(playerBulletModel_);
	FBX_SAFE_DELETE(bReticleModel_);
	FBX_SAFE_DELETE(fReticleModel_);
}

void Player::Initialize(Input* input)
{
	//���͂̃Z�b�g
	this->input_ = input;

	//���f�������w�肵�ăt�@�C���ǂݍ���
	playerModel_ = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel_ = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	bReticleModel_ = FbxLoader::GetInstance()->LoadModelFromFile("backReticle");
	fReticleModel_ = FbxLoader::GetInstance()->LoadModelFromFile("frontReticle");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	playerObject_ = new FbxObject3D;
	playerObject_->Initialize();
	playerObject_->SetModel(playerModel_);

	//�e���f���Z�b�g
	this->bulletModel_ = playerBulletModel_;

	//���e�B�N�����f���Z�b�g
	frontReticleObject_ = new FbxObject3D;
	frontReticleObject_->Initialize();
	frontReticleObject_->SetModel(fReticleModel_);

	backReticleObject_ = new FbxObject3D;
	backReticleObject_->Initialize();
	backReticleObject_->SetModel(bReticleModel_);

	//���W�A���ϊ�
	fRRotation_.y = static_cast<float>(90 * (PI / 180));
	bRRotation_.y = static_cast<float>(90 * (PI / 180));

	//�X�v���C�g
	speedSprite_ = new Sprite();
	speedSprite_->SetTextureNum(2);
	speedSprite_->Initialize();
	speedSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
}

void Player::Update()
{
	//�ړ�
	Move();

	//���e�B�N���̍X�V
	UpdateRaticle();

	//�X�v���C�g�̍X�V
	UpdateSprite();

	//�e�̍X�V
	BulletUpdate();

	if (isInvincible_) {
		invincibleTimer_++;
		if (invincibleTimer_ > InvincibleTime_) {
			isInvincible_ = false;
			invincibleTimer_ = 0;
		}
	}

	playerObject_->SetPosition(position_);
	playerObject_->SetScale(scale_);
	playerObject_->SetRotate(rotation_);
	playerObject_->Update();

	//ImGui::Begin("position");
	//ImGui::Text("%f,%f,%f",position_.x,position_.y,position_.z);
	//ImGui::End();

}


void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (isInvincible_) {

	}
	else {
		playerObject_->Draw(cmdList);
	}

	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(cmdList);
	}
}

void Player::DrawSprite(ID3D12GraphicsCommandList* cmdList)
{
	//���e�B�N��
	frontReticleObject_->Draw(cmdList);
	backReticleObject_->Draw(cmdList);

	//�X�s�[�hUI
	speedSprite_->Draw(cmdList);
}

void Player::SpeedUpByEnemy()
{
	if (addSpeed_ + addSpeedByEnemy_ >= MaxSpeed_) {
		addSpeed_ = MaxSpeed_;
	}
	else {
		addSpeed_ += addSpeedByEnemy_;
	}
}

void Player::SpeedDownByEnemy()
{
	if (!isInvincible_) {
		if (addSpeed_ - subSpeedByEnemy_ <= 0) {
			addSpeed_ = 0;
		}
		else {
			addSpeed_ -= subSpeedByEnemy_;
		}
	}
}

void Player::Move()
{

	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_W)) {
			if (MoveMax_.y > position_.y) { position_.y += speedXY_; }
		}
		else if (input_->PushKey(DIK_S)) {
			if (-MoveMax_.y < position_.y) { position_.y -= speedXY_; }
		}

		if (input_->PushKey(DIK_A)) {
			if (-MoveMax_.x < position_.x) { position_.x -= speedXY_; }
		}
		else if (input_->PushKey(DIK_D)) {
			if (MoveMax_.x > position_.x) { position_.x += speedXY_; }
		}

	}

	position_.z += speedZ_ + addSpeed_;

}

void Player::Shot()
{
	bulletCoolTimer_++;

	if (input_->PushKey(DIK_SPACE)) {

		if (BulletCoolTime_ < bulletCoolTimer_) {
			MakeBullet();

			bulletCoolTimer_ = 0;
		}
	}
}

void Player::BulletUpdate()
{
	//---�e---
	Shot();
	//�v���C���[�̒e�X�V
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Update();
	}
	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {return bullet->GetIsDead(); });
}

void Player::MakeBullet()
{
	//���@�ƃ��e�B�N���̃x�N�g�������
	Vector3 velocity = playerToReticleVec_;

	//���K�������đ��x��������
	velocity.normalize();

	bulletSpeed_ = (speedZ_ + addSpeed_);
	velocity* bulletSpeed_;

	//z���͎��@�������Ă��邽�߂��̃X�s�[�h������
	velocity.z += (speedZ_ + addSpeed_);

	//�e�̐���
	std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Player::UpdateRaticle()
{
	MoveRaticle();

	//���@�̃��[���h���W����3D���e�B�N���̃��[���h���W���v�Z
	{
		//���̃��e�B�N���̓��e�B�N�����W�����̂܂ܓ����
		fRPosition_ = reticlePosition_;

		//�����̃��e�B�N���͎��@��,���e�B�N�����W�̃x�N�g��������W���Z�o
		playerVec_ = { position_.x,position_.y,position_.z };
		reticleVec_ = { reticlePosition_.x, reticlePosition_.y, reticlePosition_.z };
		playerToReticleVec_ = reticleVec_ - playerVec_;

		//���@����̃x�N�g�������߂�
		playerToReticleVec_ = playerToReticleVec_ / (kDistancePlayerTo3DFrontReticle_ / kDistancePlayerTo3DBackReticle_);

		//���@����̃x�N�g���Ǝ��@�̍��W�𑫂�
		bRPosition_ = position_;
		bRPosition_.x += playerToReticleVec_.x;
		bRPosition_.y += playerToReticleVec_.y;
		bRPosition_.z += playerToReticleVec_.z;
	}

	//�I�u�W�F�N�g�̍X�V
	frontReticleObject_->SetPosition(fRPosition_);
	frontReticleObject_->SetRotate(fRRotation_);
	frontReticleObject_->SetScale(fRScale_);
	frontReticleObject_->Update();

	backReticleObject_->SetPosition(bRPosition_);
	backReticleObject_->SetRotate(bRRotation_);
	backReticleObject_->SetScale(bRScale_);
	backReticleObject_->Update();
}

void Player::MoveRaticle()
{
	//���e�B�N���̈ړ�

	//���@����ݒ肵�������i�񂾂Ƃ���ɍ��W��ݒ�
	reticlePosition_.z = position_.z + kDistancePlayerTo3DFrontReticle_;

	//���͂ňړ�
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_UP)) {
			if (ReticleMoveMax_.y > reticlePosition_.y) { reticlePosition_.y += reticleSpeedXY_; }
		}
		else if (input_->PushKey(DIK_DOWN)) {
			if (-ReticleMoveMax_.y < reticlePosition_.y) { reticlePosition_.y -= reticleSpeedXY_; }
		}

		if (input_->PushKey(DIK_LEFT)) {
			if (-ReticleMoveMax_.x < reticlePosition_.x) { reticlePosition_.x -= reticleSpeedXY_; }
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			if (ReticleMoveMax_.x > reticlePosition_.x) { reticlePosition_.x += reticleSpeedXY_; }
		}

	}
}

void Player::UpdateSprite()
{
	//���̃X�s�[�h���ő�X�s�[�h�̉������v�Z���X�P�[��������ɍ��킹��

	//���̃X�s�[�h(��b�X�s�[�h�͂̂���)��������
	float speedRate = addSpeed_ / MaxSpeed_;
	//�X�v���C�g�̍ő�T�C�Y
	float speedSpriteMaxSize = window_width - (speedSpriteXSpace_ * 2);

	//�ő�T�C�Y�ƍ��̃X�s�[�h�̊�����������
	speedSpriteScale_.x = speedSpriteMaxSize * speedRate;

	//�X�V
	speedSprite_->SetScale(speedSpriteScale_);
	speedSprite_->SetPosition(speedSpritePosition_);
	speedSprite_->Update();
}

void Player::Reset()
{
	//���W
	position_ = { 0,0,0 };
	//���x
	speedZ_ = 0.5f;
	addSpeed_ = 0.0f;

	//���e�B�N��
	reticlePosition_ = { 0,0,0 };
	fRPosition_ = { 0,0,0 };
	bRPosition_ = { 0,0,0 };

	//�e
	bullets_.clear();
}

CollisionData Player::GetColData()
{

	CollisionData colData;

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}

CollisionData Player::GetBulletColData(int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

void Player::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}


