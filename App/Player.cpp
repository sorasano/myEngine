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

	//3dオブジェクト生成とモデルのセット
	playerObject_ = new FbxObject3D;
	playerObject_->Initialize();
	playerObject_->SetModel(playerModel_);

	//弾モデルセット
	this->bulletModel_ = playerBulletModel_;

	//スプライト

	//レティクル
	for (int i = 0; i < reticleSpriteSize_; i++) {
		Sprite* newReticleSprite = new Sprite();
		newReticleSprite->SetTextureNum(5);
		newReticleSprite->Initialize();
		newReticleSprite->SetScale(reticleScale_);

		reticleSprites_.push_back(newReticleSprite);
	}

	//スピードUI

	speedSprite_ = new Sprite();
	speedSprite_->SetTextureNum(3);
	speedSprite_->Initialize();
	speedSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
}

void Player::Update(XMMATRIX matVP)
{
	//移動
	Move();

	//レティクルの更新
	UpdateRaticle(matVP);

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

void Player::DrawClearScene(ID3D12GraphicsCommandList* cmdList)
{
	playerObject_->Draw(cmdList);
}

void Player::DrawSprite(ID3D12GraphicsCommandList* cmdList)
{
	//レティクル
	for (Sprite*& reticleSprite : reticleSprites_) {
		reticleSprite->Draw(cmdList);
	}

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
	Vector3 playerVec = { position_.x,position_.y,position_.z };
	Vector3 reticleToPlayerVec_ = reticle3DPosition_ - playerVec;

	Vector3 velocity = reticleToPlayerVec_;

	//正規化をして速度をかける
	velocity.normalize();
	velocity *= bulletSpeed_;

	//z軸は自機も動いているためそのスピードも足す
	velocity.z += (speedZ_ + addSpeed_);

	//弾の生成
	std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Player::UpdateRaticle(XMMATRIX matVP)
{
	//マウス座標の取得、代入

	if (input_->CheckInWindow()) {
		reticle2DPosition_ = input_->GetMousePosition();
	}

	//ImGui::Begin("mouseposition");
	//ImGui::Text("%f,%f", input_->GetMousePosition().x, input_->GetMousePosition().y);
	//ImGui::End();

	//スクリーン→ワールド座標変換	
	{

		//ビューポート行列
		XMMATRIX viewPort;
		viewPort = DirectX::XMMatrixIdentity();
		viewPort.r[0].m128_f32[0] = window_width / 2;
		viewPort.r[1].m128_f32[1] = -window_height / 2;
		viewPort.r[3].m128_f32[0] = window_width / 2;
		viewPort.r[3].m128_f32[1] = window_height / 2;

		//ビュープロジェクションビュー行列
		XMMATRIX matViewProjectionViewPort = matVP * viewPort;

		//ビュープロジェクションビュー行列の逆行列を取得
		XMMATRIX matInverseVPV = XMMatrixInverse(nullptr, matViewProjectionViewPort);

		//スクリーン→ワールド座標変換
		Vector3 nearClip = { reticle2DPosition_.x , reticle2DPosition_.y ,0 };
		Vector3 farClip = { reticle2DPosition_.x , reticle2DPosition_.y ,1 };

		nearClip = XMMATRIXTransform(nearClip, matInverseVPV);
		farClip = XMMATRIXTransform(farClip, matInverseVPV);

		//マウスレイベクトル
		Vector3 mouseDirection = farClip - nearClip;
		mouseDirection.normalize();

		//ニアクリップ面上からマウスレイベクトルの方向にreticleDirection進んだ距離
		reticle3DPosition_ = nearClip + (mouseDirection * reticleDirection_);

		//プレイヤーの2d座標の算出(ワールド→スクリーン座標計算)
		Vector3 playerPosition = { position_.x,position_.y,position_.z };
		playerPosition = XMMATRIXTransform(playerPosition, matViewProjectionViewPort);
		position2D_ = { playerPosition.x,playerPosition.y};

	}

}

void Player::UpdateSprite()
{

	//スピードUIスプライト
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

	//レティクルスプライト
	{
		//プレイヤーとレティクルの2Dベクトル
		XMFLOAT2 plyerToReticle2D = { reticle2DPosition_.x - position2D_.x , reticle2DPosition_.y - position2D_.y };

		//レティクルの間隔(等分)
		XMFLOAT2 reticleSplitPos = { plyerToReticle2D.x / reticleSpriteSize_, plyerToReticle2D.y / reticleSpriteSize_ };

		//レティクルのサイズ減少値
		float reticleSplitSize = 15.0f;

		for (int i = 0; i < reticleSpriteSize_; i++) {
			//プレイヤーの座標から等分に配置
			reticleSprites_[i]->SetPosition({ position2D_.x + reticleSplitPos.x * (i + 1), position2D_.y + reticleSplitPos.y * (i + 1) });
			//手前は大きく奥はちいさく
			reticleSprites_[i]->SetScale({reticleScale_.x - (reticleSplitSize * (i + 1)) ,reticleScale_.y - (reticleSplitSize * (i + 1)) });

			reticleSprites_[i]->Update();
		}
	}

}

void Player::Reset()
{
	//座標
	position_ = { 0,0,0 };
	//速度
	speedZ_ = 0.5f;
	addSpeed_ = 0.0f;

	//レティクル
	//reticlePosition_ = { 0,0 };

	//弾
	bullets_.clear();
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



