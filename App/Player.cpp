/**
* @file Player.cpp
* @brief プレイヤーの処理
*/

#include "Player.h"
#include "Imgui.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	//入力のセット
	this->input_ = MyEngine::Input::GetInstance();

	//モデル名を指定してファイル読み込み
	playerModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("player"));
	bulletModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("playerBullet"));

	//3dオブジェクト生成とモデルのセット
	std::unique_ptr<FbxObject3D> newPlayerObject = std::make_unique<FbxObject3D>();
	newPlayerObject->Initialize();
	newPlayerObject->SetModel(playerModel_.get());
	playerObject_.swap(newPlayerObject);

	//モデルの角度調整
	this->rotation_.x = static_cast<float>(90 * (PI / 180));
	this->rotation_.y = static_cast<float>(90 * (PI / 180));


	//スプライト

	//レティクル
	for (int i = 0; i < reticleSpriteSize_; i++) {
		std::unique_ptr<Sprite> newReticleSprite = 
			std::make_unique<Sprite>(
				5,
				reticleScale_,
				XMFLOAT2(0,0)
			);
		reticleSprites_.push_back(std::move(newReticleSprite));
	}

	//スピードUI

	std::unique_ptr<Sprite> newMainSpeedSprite_ = std::make_unique<Sprite>();
	newMainSpeedSprite_->SetTextureNum(3);
	newMainSpeedSprite_->Initialize();
	mainSpeedSprite_.swap(newMainSpeedSprite_);

	std::unique_ptr<Sprite> newSubSpeedSprite_ = std::make_unique<Sprite>();
	newSubSpeedSprite_->SetTextureNum(16);
	newSubSpeedSprite_->Initialize();
	subSpeedSprite_.swap(newSubSpeedSprite_);

	speedGaugeSpriteScale_.x = speedSpriteMaxSize;
	std::unique_ptr<Sprite> newSpeedGaugeSprite_ = 
		std::make_unique<Sprite>(
			17,
			speedGaugeSpriteScale_,
			speedGaugeSpritePosition_
		);
	speedGaugeSprite_.swap(newSpeedGaugeSprite_);

	Reset();
}

void Player::Update(const XMMATRIX& matVP)
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
	position_.z += speedZ_ + mainSpeed_;

	UpdateMatrix();
}

void Player::UpdateGameoverScene()
{
	//移動
	position_.z += speedZ_;

	UpdateMatrix();

	//弾があったら削除
	if (bullets_.size()) {
		bullets_.clear();
	}
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
	for (std::unique_ptr<Sprite>& reticleSprite : reticleSprites_) {
		reticleSprite->Draw(cmdList);
	}

	//スピードUI
	mainSpeedSprite_->Draw(cmdList);

	subSpeedSprite_->Draw(cmdList);

	speedGaugeSprite_->Draw(cmdList);

}

void Player::SpeedUpByEnemy()
{
	//スピードを加速するとサブスピードがマックスになるか
	if (subSpeed_ + subUpSpeed_ >= SubMaxSpeed_) {

		//サブスピードがマックスになったらメインスピードを上げ、サブスピードを0に
		mainSpeed_ += mainUpSpeed_;
		subSpeed_ = 0;

	}
	else {
		//ならなければそのまま加速
		subSpeed_ += subUpSpeed_;
	}


}

void Player::SpeedDownByEnemy()
{
	if (!isInvincible_) {

		//減速するとサブスピードが0以下になるか
		if (subSpeed_ - subDownSpeed_ <= 0) {

			//メインスピードがあるかどうか
			if (mainSpeed_ ) {

				//メインスピードを減速した場合0以下になるか
				if (mainSpeed_ - mainDownSpeed_ <= 0) {

					//なるならメインスピードを0にしサブスピードをマックスに
					mainSpeed_ = 0;
					subSpeed_ = SubMaxSpeed_;

				}
				else {
					//ならなければそのまま減速しサブスピードをマックスに
					mainSpeed_ -= mainDownSpeed_;
					subSpeed_ = SubMaxSpeed_;
				}

			}
			else {
				//ない場合サブスピードは0に
				subSpeed_ = 0;
			}
		}
		else {
			//ならない場合そのまま減速
			subSpeed_ -= subDownSpeed_;
		}
	}
}

void Player::Move()
{
	//ベクトルのリセット
	moveVec_.zero();

	//キーボード操作
	if (!isLockOperation_) {

		if (input_->IsKeyPress(DIK_W) || input_->IsKeyPress(DIK_S) || input_->IsKeyPress(DIK_D) || input_->IsKeyPress(DIK_A)) {

			//座標を移動する処理
			if (input_->IsKeyPress(DIK_W)) {
				if (MoveMax_.y > position_.y) { moveVec_.y = +speedXY_; }
			}
			else if (input_->IsKeyPress(DIK_S)) {
				if (-MoveMax_.y < position_.y) { moveVec_.y = -speedXY_; }
			}

			if (input_->IsKeyPress(DIK_A)) {
				if (-MoveMax_.x < position_.x) { moveVec_.x = -speedXY_; }
			}
			else if (input_->IsKeyPress(DIK_D)) {
				if (MoveMax_.x > position_.x) { moveVec_.x = +speedXY_; }
			}

		}
	}

	//奥移動
	moveVec_.z = speedZ_ + mainSpeed_;

	//ベクトルの加算
	VecAddXMFLOAT3(position_,moveVec_);

}

void Player::Shot()
{
	bulletCoolTimer_++;
	//クリックで弾発射
	if (input_->IsMousePress(LEFT_CLICK) && !isLockOperation_) {

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
	velocity.z += (speedZ_ + mainSpeed_);
	//弾の生成
	std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
	newBullet->Initialize(bulletModel_.get(), position_, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Player::UpdateRaticle(const XMMATRIX& matVP)
{
	//マウス座標の取得、代入
	if (input_->CheckInWindow()) {
		reticle2DPosition_ = input_->GetMousePosition();
	}

	//座標変換
	ScreenToWorldCoordinateTransformation(matVP);
}

void Player::UpdateSprite()
{

	//スピードゲージUI
	speedGaugeSprite_->Update();

	//スピードUIスプライト(サブ)
	{
		//今のスピードが最大スピードの何割か計算しスケールをそれに合わせる

		//今のスピード(基礎スピードはのぞく)が何割か
		float speedRate = subSpeed_ / SubMaxSpeed_;

		//最大サイズと今のスピードの割合をかける
		subSpeedSpriteScale_.x = speedSpriteMaxSize * speedRate;

		//更新
		subSpeedSprite_->SetScale(subSpeedSpriteScale_);
		subSpeedSprite_->SetPosition(subSpeedSpritePosition_);
		subSpeedSprite_->Update();
	}

	//スピードUIスプライト(メイン)
	{
		//今のスピードが最大スピードの何割か計算しスケールをそれに合わせる

		//今のスピード(基礎スピードはのぞく)が何割か
		float speedRate = mainSpeed_ / MainMaxSpeed_;

		//最大サイズと今のスピードの割合をかける
		mainSpeedSpriteScale_.x = speedSpriteMaxSize * speedRate;

		//更新
		mainSpeedSprite_->SetScale(mainSpeedSpriteScale_);
		mainSpeedSprite_->SetPosition(mainSpeedSpritePosition_);
		mainSpeedSprite_->Update();
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
			reticleSprites_[i]->SetScale({ reticleScale_.x - (reticleSplitSize * (i + 1)) ,reticleScale_.y - (reticleSplitSize * (i + 1)) });

			reticleSprites_[i]->Update();
		}
	}

}

void Player::ScreenToWorldCoordinateTransformation(const XMMATRIX& matVP)
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
	XMMATRIX matInverseVPV = MatrixInverse(matViewProjectionViewPort);

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
	position2D_ = { playerPosition.x,playerPosition.y };

}

void Player::Reset()
{
	//座標
	position_ = initPosition_;
	//速度
	speedZ_ = initSpeedZ_;
	mainSpeed_ = initMainAddSpeed_;
	subSpeed_ = initSubAddSpeed_;

	//弾
	bullets_.clear();
}

CollisionData Player::GetColData() const
{

	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}

CollisionData Player::GetBulletColData(int i) const
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

XMFLOAT3 Player::GetBulletPosition(int i) const
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