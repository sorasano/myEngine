/**
* @file Enemy.cpp
* @brief 敵の処理
*/

#include "Enemy.h"
#include "Random.h"
#include "Imgui.h"

Enemy* Enemy::GetInstance()
{
	static Enemy instance;
	return &instance;
}

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize(FbxModel* EnemyModel,FbxModel* enemyBulletModel)
{

	//3dオブジェクト生成とモデルのセット
	std::unique_ptr<FbxObject3D> newEnemyObject_ = std::make_unique<FbxObject3D>();
	newEnemyObject_->Initialize();
	newEnemyObject_->SetModel(EnemyModel);
	enemyObject_.swap(newEnemyObject_);

	this->bulletModel_ = enemyBulletModel;

}

void Enemy::Update(const XMFLOAT3& pPos, float pSpeed, const XMMATRIX& matVP)
{
	//プレイヤー情報更新
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead_) {

		//画面内に停滞させる
		StopInScreen();

		//移動
		if (moveType_ != NOTMOVE) {
			Move();
		}

		//射撃
		if (shotType_ != NOTSHOT) {
			//プレイヤーのスピードで発射し始める座標を変更
			shotStartPos_ = ShotStart_ * playerSpeed_;

			if (position_.z < playerPosition_.z + shotStartPos_) {
				BulletUpdate();
			}
		}

		//ホーミング敵のみの更新
		if (shotType_ == HOMING) {
			HomingUpdate();
		}
	}

	enemyObject_->SetPosition(position_);
	enemyObject_->SetScale(scale_);
	enemyObject_->SetRotate(rotation_);
	enemyObject_->Update();

	//2D座標の取得
	WorldToScreenTransformation(matVP);
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead_) {
		enemyObject_->Draw(cmdList);

		//弾
		for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}

}

void Enemy::WorldToScreenTransformation(const XMMATRIX& matVP)
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

	//プレイヤーの2d座標の算出(ワールド→スクリーン座標計算)
	Vector3 enemyPosition = { position_.x,position_.y,position_.z };
	enemyPosition = XMMATRIXTransform(enemyPosition, matViewProjectionViewPort);
	position2D_ = { enemyPosition.x,enemyPosition.y };
}

void Enemy::Move()
{
	//ベクトルのリセット
	moveVec_.zero();

	switch (moveType_)
	{
	case NOTMOVE:
		break;

	case MOVEX:
		MoveX();
		break;

	case MOVEY:
		MoveY();
		break;

	case MOVEDIA:
		MoveX();
		MoveY();
		break;
	}

	//ベクトルの加算
	VecAddXMFLOAT3(position_, moveVec_);
}

void Enemy::MoveX()
{
	if (moveX_) {

		if (position_.x < moveMaxX_) {
			moveVec_.x = +moveSpeed_;
		}
		else {
			moveX_ = false;
		}

	}
	else {

		if (position_.x > -moveMaxX_) {
			moveVec_.x = -moveSpeed_;
		}
		else {
			moveX_= true;
		}
	}
}

void Enemy::MoveY()
{
	if (moveY_) {

		if (position_.y < moveMaxY_) {
			moveVec_.y = +moveSpeed_;
		}
		else {
			moveY_ = false;
		}

	}
	else {

		if (position_.y > -moveMaxY_) {
			moveVec_.y = -moveSpeed_;
		}
		else {
			moveY_ = true;
		}
	}
}

void Enemy::Reflection()
{
	//移動を反対向きにさせる
	if (moveType_ != NOTMOVE) {
		if (moveX_) {
			moveX_ = false;
		}
		else {
			moveX_= true;
		}

		if (moveY_) {
			moveY_ = false;
		}
		else {
			moveY_ = true;
		}
	}
}

void Enemy::StopInScreen()
{

	//stopInScreenがtrueの場合、プレイヤーからstopInScreenPosition進んだ距離に到達したら停滞フラグをオンに
	if (stopInScreen_ && position_.z < playerPosition_.z + stopInScreenPosition_) {
		isStopInScreen_ = true;
		stopInScreen_ = false;
	}

	//停滞フラグがオンの場合
	if(isStopInScreen_){

		//自機についていく
		position_.z += playerSpeed_;

		//タイマーを進める
		stopInScreenTimer_++;
		if (stopInScreenTimer_ > StopInScreenTime_) {
			isStopInScreen_ = false;
		}
	}

}

void Enemy::HomingUpdate()
{
	Vector3 velocity = GetEnemyToPlayerVec();

	//角度をベクトルから求めてモデルに反映
	//Y軸周り角度
	this->rotation_.y = std::atan2(velocity.x, velocity.z - playerSpeed_);

	//横軸方向の長さを求める
	Vector3 velocityXZ = { velocity.x,0.0f,velocity.z - playerSpeed_ };
	float length = velocityXZ.length();

	//X軸周り角度
	this->rotation_.x = std::atan2(-velocity.y, length);

	//ImGui::Begin("aaa");
	//ImGui::Text("%f,%f,%f", rotation_.x, rotation_.y, rotation_.z);
	//ImGui::End();

}

Vector3 Enemy::GetEnemyToPlayerVec()
{

	Vector3 velocity = {};

	//自機と敵のベクトルを取る
	Vector3 playerVec_ = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
	Vector3 enemyVec = { position_.x,position_.y,position_.z + playerSpeed_ };

	velocity = playerVec_ - enemyVec;

	//正規化する
	velocity.normalize();
	//速度をかける
	velocity *= bulletSpeed_;
	velocity.z += playerSpeed_;

	return velocity;
}

void Enemy::Shot()
{
	bulletCoolTimer_++;

	if (BulletCoolTime_ < bulletCoolTimer_) {
		MakeBullet();

		bulletCoolTimer_ = 0;
	}

}

void Enemy::BulletUpdate()
{
	//プレイヤーより前にいる敵の弾のみ発射
	if (position_.z > playerPosition_.z) {
		Shot();
	}

	//敵の弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {return bullet->GetIsDead(); });
}

void Enemy::MakeBullet()
{
	//射出ベクトル
	Vector3 velocity = {};

	switch (shotType_)
	{
	case NOTSHOT:

		break;

	case STRAIGHTSHOT:
		//z軸の-方向の単位ベクトルに速度をかける
		velocity = { 0.0f,0.0f,-1.0f };
		velocity *= bulletSpeed_;
		velocity.z += playerSpeed_;

		break;

	case HOMINGSHOT:

		velocity = GetEnemyToPlayerVec();

		break;
	}

	//弾の生成
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity, playerSpeed_);
	bullets_.push_back(std::move(newBullet));
}

CollisionData Enemy::GetColData() const
{

	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Enemy::SetModel(FbxModel* EnemyModel)
{
	enemyObject_->SetModel(EnemyModel);
}

void Enemy::SetType(int type)
{

	this->type_ = type;

	switch (type_)
	{
	case NOTHING:
		shotType_ = NOTSHOT;
		moveType_ = NOTMOVE;
		break;

	case NORMAL:
		shotType_ = STRAIGHTSHOT;
		moveType_ = NOTMOVE;
		break;

	case HOMING:
		shotType_ = HOMINGSHOT;
		moveType_ = NOTMOVE;
		break;

	case MOVINGX:
		shotType_= STRAIGHTSHOT;
		moveType_ = MOVEX;
		break;

	case MOVINGY:
		shotType_ = STRAIGHTSHOT;
		moveType_= MOVEY;
		break;

	case MOVINGDIA:
		shotType_ = STRAIGHTSHOT;
		moveType_ = MOVEDIA;
		break;
	}
}

CollisionData Enemy::GetBulletColData(int i) const
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

XMFLOAT3 Enemy::GetBulletPosition(int i) const
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetPosition();
}

void Enemy::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}

