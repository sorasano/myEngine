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
	FBX_SAFE_DELETE(playerObject);
	FBX_SAFE_DELETE(playerModel);
	FBX_SAFE_DELETE(playerBulletModel);
	FBX_SAFE_DELETE(bReticleModel);
	FBX_SAFE_DELETE(fReticleModel);
}

void Player::Initialize(Input* input)
{
	//���͂̃Z�b�g
	this->input_ = input;

	//���f�������w�肵�ăt�@�C���ǂݍ���
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	bReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("backReticle");
	fReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("frontReticle");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	playerObject = new FbxObject3D;
	playerObject->Initialize();
	playerObject->SetModel(playerModel);

	//�e���f���Z�b�g
	this->bulletModel_ = playerBulletModel;

	//���e�B�N�����f���Z�b�g
	frontReticleObject = new FbxObject3D;
	frontReticleObject->Initialize();
	frontReticleObject->SetModel(fReticleModel);

	backReticleObject = new FbxObject3D;
	backReticleObject->Initialize();
	backReticleObject->SetModel(bReticleModel);

	//���W�A���ϊ�
	fRRotation_.y = static_cast<float>(90 * (PI / 180));
	bRRotation_.y = static_cast<float>(90 * (PI / 180));

}

void Player::Update()
{
	//�ړ�
	Move();

	//���e�B�N���̍X�V
	UpdateRaticle();

	//�e�̍X�V
	BulletUpdate();

	if (isInvincible) {
		invincibleTimer++;
		if (invincibleTimer > InvincibleTime) {
			isInvincible = false;
			invincibleTimer = 0;
		}
	}

	playerObject->SetPosition(position_);
	playerObject->SetScale(scale_);
	playerObject->SetRotate(rotation_);
	playerObject->Update();

	//ImGui::Begin("position");
	//ImGui::Text("%f,%f,%f",position_.x,position_.y,position_.z);
	//ImGui::End();

}


void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (isInvincible) {

	}
	else {
		playerObject->Draw(cmdList);
	}

	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(cmdList);
	}
}

void Player::DrawRaticle(ID3D12GraphicsCommandList* cmdList)
{
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
	Vector3 velocity = playerToReticleVec;

	//���K�������đ��x��������
	velocity.normalize();

	bulletSpeed = (speedZ + addSpeed);
	velocity * bulletSpeed;

	//z���͎��@�������Ă��邽�߂��̃X�s�[�h������
	velocity.z += (speedZ + addSpeed);

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
		playerVec = { position_.x,position_.y,position_.z };
		reticleVec = { reticlePosition_.x, reticlePosition_.y, reticlePosition_.z };
		playerToReticleVec = reticleVec - playerVec;

		//���@����̃x�N�g�������߂�
		playerToReticleVec = playerToReticleVec / (kDistancePlayerTo3DFrontReticle / kDistancePlayerTo3DBackReticle);

		//���@����̃x�N�g���Ǝ��@�̍��W�𑫂�
		bRPosition_ = position_;
		bRPosition_.x += playerToReticleVec.x;
		bRPosition_.y += playerToReticleVec.y;
		bRPosition_.z += playerToReticleVec.z;
	}

	//�I�u�W�F�N�g�̍X�V
	frontReticleObject->SetPosition(fRPosition_);
	frontReticleObject->SetRotate(fRRotation_);
	frontReticleObject->SetScale(fRScale_);
	frontReticleObject->Update();

	backReticleObject->SetPosition(bRPosition_);
	backReticleObject->SetRotate(bRRotation_);
	backReticleObject->SetScale(bRScale_);
	backReticleObject->Update();
}

void Player::MoveRaticle()
{
	//���e�B�N���̈ړ�

	//���@����ݒ肵�������i�񂾂Ƃ���ɍ��W��ݒ�
	reticlePosition_.z = position_.z + kDistancePlayerTo3DFrontReticle;

	//���͂ňړ�
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_UP)) {
			if (ReticleMoveMax.y > reticlePosition_.y) { reticlePosition_.y += reticleSpeedXY; }
		}
		else if (input_->PushKey(DIK_DOWN)) {
			if (-ReticleMoveMax.y < reticlePosition_.y) { reticlePosition_.y -= reticleSpeedXY; }
		}

		if (input_->PushKey(DIK_LEFT)) {
			if (-ReticleMoveMax.x < reticlePosition_.x) { reticlePosition_.x -= reticleSpeedXY; }
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			if (ReticleMoveMax.x > reticlePosition_.x) { reticlePosition_.x += reticleSpeedXY; }
		}

	}
}

CollisionData Player::GetColData()
{

	CollisionData colData;

	colData.position = this->position_;
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


