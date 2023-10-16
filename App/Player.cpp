/**
* @file Player.cpp
* @brief プレイヤーの処理
*/

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
	//入力のセット
	this->input_ = input;

	//モデル名を指定してファイル読み込み
	playerModel_ = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel_ = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	bReticleModel_ = FbxLoader::GetInstance()->LoadModelFromFile("backReticle");
	fReticleModel_ = FbxLoader::GetInstance()->LoadModelFromFile("frontReticle");

	//3dオブジェクト生成とモデルのセット
	playerObject_ = new FbxObject3D;
	playerObject_->Initialize();
	playerObject_->SetModel(playerModel_);

	//弾モデルセット
	this->bulletModel_ = playerBulletModel_;

	//レティクルモデルセット
	frontReticleObject_ = new FbxObject3D;
	frontReticleObject_->Initialize();
	frontReticleObject_->SetModel(fReticleModel_);

	backReticleObject_ = new FbxObject3D;
	backReticleObject_->Initialize();
	backReticleObject_->SetModel(bReticleModel_);

	//ラジアン変換
	fRRotation_.y = static_cast<float>(90 * (PI / 180));
	bRRotation_.y = static_cast<float>(90 * (PI / 180));

	//スプライト
	speedSprite_ = new Sprite();
	speedSprite_->SetTextureNum(2);
	speedSprite_->Initialize();
	speedSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
}

void Player::Update()
{
	//移動
	Move();

	//レティクルの更新
	UpdateRaticle();

	//スプライトの更新
	UpdateSprite();

	//弾の更新
	BulletUpdate();

	if (isInvincible_) {
		invincibleTimer_++;
		if (invincibleTimer_ > InvincibleTime_) {
			isInvincible_ = false;
			invincibleTimer_ = 0;
		}
	}

	//isInvincible_ = true;

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

	//弾
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(cmdList);
	}
}

void Player::DrawSprite(ID3D12GraphicsCommandList* cmdList)
{
	//レティクル
	frontReticleObject_->Draw(cmdList);
	backReticleObject_->Draw(cmdList);

	//スピードUI
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

		//座標を移動する処理
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
	//---弾---
	Shot();
	//プレイヤーの弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Update();
	}
	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {return bullet->GetIsDead(); });
}

void Player::MakeBullet()
{
	//自機とレティクルのベクトルを取る
	Vector3 velocity = playerToReticleVec_;

	//正規化をして速度をかける
	velocity.normalize();

	bulletSpeed_ = (speedZ_ + addSpeed_);
	velocity* bulletSpeed_;

	//z軸は自機も動いているためそのスピードも足す
	velocity.z += (speedZ_ + addSpeed_);

	//弾の生成
	std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Player::UpdateRaticle()
{
	MoveRaticle();

	//自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		//奥のレティクルはレティクル座標をそのまま入れる
		fRPosition_ = reticlePosition_;

		//中央のレティクルは自機と,レティクル座標のベクトルから座標を算出
		playerVec_ = { position_.x,position_.y,position_.z };
		reticleVec_ = { reticlePosition_.x, reticlePosition_.y, reticlePosition_.z };
		playerToReticleVec_ = reticleVec_ - playerVec_;

		//自機からのベクトルを求める
		playerToReticleVec_ = playerToReticleVec_ / (kDistancePlayerTo3DFrontReticle_ / kDistancePlayerTo3DBackReticle_);

		//自機からのベクトルと自機の座標を足す
		bRPosition_ = position_;
		bRPosition_.x += playerToReticleVec_.x;
		bRPosition_.y += playerToReticleVec_.y;
		bRPosition_.z += playerToReticleVec_.z;
	}

	//オブジェクトの更新
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
	//レティクルの移動

	//自機から設定した距離進んだところに座標を設定
	reticlePosition_.z = position_.z + kDistancePlayerTo3DFrontReticle_;

	//入力で移動
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//座標を移動する処理
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
	//今のスピードが最大スピードの何割か計算しスケールをそれに合わせる

	//今のスピード(基礎スピードはのぞく)が何割か
	float speedRate = addSpeed_ / MaxSpeed_;
	//スプライトの最大サイズ
	float speedSpriteMaxSize = window_width - (speedSpriteXSpace_ * 2);

	//最大サイズと今のスピードの割合をかける
	speedSpriteScale_.x = speedSpriteMaxSize * speedRate;

	//更新
	speedSprite_->SetScale(speedSpriteScale_);
	speedSprite_->SetPosition(speedSpritePosition_);
	speedSprite_->Update();
}

void Player::Reset()
{
	//座標
	position_ = { 0,0,0 };
	//速度
	speedZ_ = 0.5f;
	addSpeed_ = 0.0f;

	//レティクル
	reticlePosition_ = { 0,0,0 };
	fRPosition_ = { 0,0,0 };
	bRPosition_ = { 0,0,0 };

	//弾
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

