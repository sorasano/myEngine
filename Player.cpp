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
	//入力のセット
	this->input_ = input;

	//モデル名を指定してファイル読み込み
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	bReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("backReticle");
	fReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("frontReticle");

	//3dオブジェクト生成とモデルのセット
	playerObject = new FbxObject3D;
	playerObject->Initialize();
	playerObject->SetModel(playerModel);

	//弾モデルセット
	this->bulletModel_ = playerBulletModel;

	//レティクルモデルセット
	frontReticleObject = new FbxObject3D;
	frontReticleObject->Initialize();
	frontReticleObject->SetModel(fReticleModel);

	backReticleObject = new FbxObject3D;
	backReticleObject->Initialize();
	backReticleObject->SetModel(bReticleModel);

	//ラジアン変換
	fRRotation_.y = static_cast<float>(90 * (PI / 180));
	bRRotation_.y = static_cast<float>(90 * (PI / 180));

}

void Player::Update()
{
	//移動
	Move();

	//レティクルの更新
	UpdateRaticle();

	//弾の更新
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

	//弾
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_)
	{
		bullet->Draw(cmdList);
	}
}

void Player::DrawRaticle(ID3D12GraphicsCommandList* cmdList)
{
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
	Vector3 velocity = playerToReticleVec;

	//正規化をして速度をかける
	velocity.normalize();

	bulletSpeed = (speedZ + addSpeed);
	velocity * bulletSpeed;

	//z軸は自機も動いているためそのスピードも足す
	velocity.z += (speedZ + addSpeed);

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
		playerVec = { position_.x,position_.y,position_.z };
		reticleVec = { reticlePosition_.x, reticlePosition_.y, reticlePosition_.z };
		playerToReticleVec = reticleVec - playerVec;

		//自機からのベクトルを求める
		playerToReticleVec = playerToReticleVec / (kDistancePlayerTo3DFrontReticle / kDistancePlayerTo3DBackReticle);

		//自機からのベクトルと自機の座標を足す
		bRPosition_ = position_;
		bRPosition_.x += playerToReticleVec.x;
		bRPosition_.y += playerToReticleVec.y;
		bRPosition_.z += playerToReticleVec.z;
	}

	//オブジェクトの更新
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
	//レティクルの移動

	//自機から設定した距離進んだところに座標を設定
	reticlePosition_.z = position_.z + kDistancePlayerTo3DFrontReticle;

	//入力で移動
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//座標を移動する処理
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


