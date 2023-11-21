/**
* @file Player.cpp
* @brief プレイヤーの処理
*/

#include "Player.h"
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
}

void Player::Initialize()
{
	//入力のセット
	this->input_ = Input::GetInstance();

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

	////ラジアン変換
	//fRRotation_.y = static_cast<float>(90 * (PI / 180));
	//bRRotation_.y = static_cast<float>(90 * (PI / 180));

	//スプライト

	//レティクル
	reticleSprite_ = new Sprite();
	reticleSprite_->SetTextureNum(5);
	reticleSprite_->Initialize();
	reticleSprite_->SetScale(XMFLOAT2(64, 64));

	//スピードUI
	speedSprite_ = new Sprite();
	speedSprite_->SetTextureNum(3);
	speedSprite_->Initialize();
	speedSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
}

void Player::Update()
{
	//移動
	Move();

	//レティクルの更新
	UpdateRaticle();

	//弾の更新
	BulletUpdate();

	//無敵時間更新
	if (isInvincible_) {
		invincibleTimer_++;
		if (invincibleTimer_ > InvincibleTime_) {
			isInvincible_ = false;
			invincibleTimer_ = 0;
		}
	}

	//スプライトの更新
	UpdateSprite();

	UpdateMatrix();

}

void Player::UpdateMatrix()
{
	playerObject_->SetPosition(position_);
	playerObject_->SetScale(scale_);
	playerObject_->SetRotate(rotation_);
	playerObject_->Update();
}

void Player::UpdateClearScene()
{
	//移動
	position_.z += speedZ_ + addSpeed_;

	UpdateMatrix();
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
	reticleSprite_->Draw(cmdList);
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
	//キーボード操作
	if (input_->IsKeyPress(DIK_W) || input_->IsKeyPress(DIK_S) || input_->IsKeyPress(DIK_D) || input_->IsKeyPress(DIK_A)) {

		//座標を移動する処理
		if (input_->IsKeyPress(DIK_W)) {
			if (MoveMax_.y > position_.y) { position_.y += speedXY_; }
		}
		else if (input_->IsKeyPress(DIK_S)) {
			if (-MoveMax_.y < position_.y) { position_.y -= speedXY_; }
		}

		if (input_->IsKeyPress(DIK_A)) {
			if (-MoveMax_.x < position_.x) { position_.x -= speedXY_; }
		}
		else if (input_->IsKeyPress(DIK_D)) {
			if (MoveMax_.x > position_.x) { position_.x += speedXY_; }
		}

	}

	//パッド操作
	else if (input_->IsDownLStickUp() || input_->IsDownLStickDown() || input_->IsDownLStickRight() || input_->IsDownLStickLeft()) {

		//座標を移動する処理
		if (input_->IsDownLStickUp()) {
			if (MoveMax_.y > position_.y) { position_.y += speedXY_; }
		}
		else if (input_->IsDownLStickDown()) {
			if (-MoveMax_.y < position_.y) { position_.y -= speedXY_; }
		}

		if (input_->IsDownLStickLeft()) {
			if (-MoveMax_.x < position_.x) { position_.x -= speedXY_; }
		}
		else if (input_->IsDownLStickRight()) {
			if (MoveMax_.x > position_.x) { position_.x += speedXY_; }
		}

	}

	position_.z += speedZ_ + addSpeed_;

}

void Player::Shot()
{
	bulletCoolTimer_++;
	//クリックで弾発射
	if (input_->IsMousePress(LEFT_CLICK)) {

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

	//自機とレティクルのベクトルを算出
	{
		playerVec_ = { position_.x,position_.y,position_.z };
		reticleVec_ = { reticle3DPosition_.x, reticle3DPosition_.y, reticle3DPosition_.z };
		playerToReticleVec_ = reticleVec_ - playerVec_;
	}

	//ワールド→スクリーン座標変換	
	{
		//ビューポート行列(スクリーン行列)
		Matrix4 matViewport = {
			ReticleMoveMax_.x / 2,						0,											0,0,
			0,											-ReticleMoveMax_.y / 2,						0,0,
			0,											0,											1,0,
			ReticleMoveMax_.x / 2 + window_width / 2,	ReticleMoveMax_.y / 2 + window_height / 2,	0,1
		};

		//ワールド→スクリーン座標変換
		reticleVec_ = Transform(reticleVec_, matViewport);

	}

	////オブジェクトの更新
	reticleSprite_->SetPosition(XMFLOAT2(reticleVec_.x, reticleVec_.y));
	reticleSprite_->Update();

	XMFLOAT2 mousePosition;
	mousePosition = input_->GetMousePosition();

	ImGui::Begin("mouseposition");
	ImGui::Text("%f,%f", mousePosition.x, mousePosition.y);
	ImGui::End();
}

void Player::MoveRaticle()
{
	//レティクルの移動

	//自機から設定した距離進んだところに座標を設定
	reticle3DPosition_.z = position_.z + kDistancePlayerToReticle_;

	//入力で移動
	if (input_->IsKeyPress(DIK_UP) || input_->IsKeyPress(DIK_DOWN) || input_->IsKeyPress(DIK_RIGHT) || input_->IsKeyPress(DIK_LEFT)) {

		//座標を移動する処理
		if (input_->IsKeyPress(DIK_UP)) {
			if (ReticleMoveMax_.y > reticle3DPosition_.y) { reticle3DPosition_.y += reticleSpeedXY_; }
		}
		else if (input_->IsKeyPress(DIK_DOWN)) {
			if (-ReticleMoveMax_.y < reticle3DPosition_.y) { reticle3DPosition_.y -= reticleSpeedXY_; }
		}

		if (input_->IsKeyPress(DIK_LEFT)) {
			if (-ReticleMoveMax_.x < reticle3DPosition_.x) { reticle3DPosition_.x -= reticleSpeedXY_; }
		}
		else if (input_->IsKeyPress(DIK_RIGHT)) {
			if (ReticleMoveMax_.x > reticle3DPosition_.x) { reticle3DPosition_.x += reticleSpeedXY_; }
		}

	}

	//入力で移動(コントローラー)
	else if (input_->IsDownRStickUp() || input_->IsDownRStickDown() || input_->IsDownRStickRight() || input_->IsDownRStickLeft()) {

		//座標を移動する処理
		if (input_->IsDownRStickUp()) {
			if (ReticleMoveMax_.y > reticle3DPosition_.y) { reticle3DPosition_.y += reticleSpeedXY_; }
		}
		else if (input_->IsDownRStickDown()) {
			if (-ReticleMoveMax_.y < reticle3DPosition_.y) { reticle3DPosition_.y -= reticleSpeedXY_; }
		}

		if (input_->IsDownRStickLeft()) {
			if (-ReticleMoveMax_.x < reticle3DPosition_.x) { reticle3DPosition_.x -= reticleSpeedXY_; }
		}
		else if (input_->IsDownRStickRight()) {
			if (ReticleMoveMax_.x > reticle3DPosition_.x) { reticle3DPosition_.x += reticleSpeedXY_; }
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
	reticle3DPosition_ = { 0,0,0 };

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

XMFLOAT3 Player::GetBulletPosition(int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetPosition();
}

void Player::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}



