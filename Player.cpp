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
	//入力のセット
	this->input_ = input;

	//3dオブジェクト生成とモデルのセット
	playerObject = new FbxObject3D;
	playerObject->Initialize();
	playerObject->SetModel(playerModel);

	//弾モデルセット
	this->playerBulletModel_ = playerBulletModel;

	//レティクルモデルセット
	frontReticleObject = new FbxObject3D;
	frontReticleObject->Initialize();
	frontReticleObject->SetModel(fRModel);

	backReticleObject = new FbxObject3D;
	backReticleObject->Initialize();
	backReticleObject->SetModel(bRModel);

	//ラジアン変換
	fRRotation_.y = static_cast<float>(90 * (PI / 180));
	bRRotation_.y = static_cast<float>(90 * (PI / 180));

}

void Player::Update()
{
	//移動
	Move();

	//弾の更新
	BulletUpdate();

	if (isInvincible) {
		invincibleTimer++;
		if (invincibleTimer > InvincibleTime) {
			isInvincible = false;
			invincibleTimer = 0;
		}
	}

	//レティクルの更新
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

	//弾
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullet_)
	{
		bullet->Draw(cmdList);
	}

	//レティクル
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

		//座標を移動する処理
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
	//---弾---
	Shot();
	//プレイヤーの弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullet_)
	{
		bullet->Update();
	}
	//デスフラグの立った弾を削除
	playerBullet_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {return bullet->GetIsDead(); });
}

void Player::MakeBullet()
{
	//弾の生成
	std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
	newBullet->Initialize(playerBulletModel_, position_, speedZ+addSpeed);
	playerBullet_.push_back(std::move(newBullet));
}

void Player::UpdateRaticle()
{
	//自機のワールド座標から3Dレティクルのワールド座標を計算
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


