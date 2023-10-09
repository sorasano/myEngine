#include "Enemy.h"
#include "Random.h"

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
	FBX_SAFE_DELETE(enemyObject_);
}

void Enemy::Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel_)
{

	//3dオブジェクト生成とモデルのセット
	enemyObject_ = new FbxObject3D;
	enemyObject_->Initialize();
	enemyObject_->SetModel(EnemyModel);

	this->bulletModel_ = enemyBulletModel_;

}

void Enemy::Update(XMFLOAT3 pPos, float pSpeed)
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
	}
	//パーティクル
	else if (isParticle_) {
		UpdateParticle();
	}

	enemyObject_->SetPosition(position_);
	enemyObject_->SetScale(scale_);
	enemyObject_->SetRotate(rotation_);
	enemyObject_->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList_)
{
	if (!isDead_) {
		enemyObject_->Draw(cmdList_);

		//弾
		for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList_);
		}
	}
	//----パーティクル----
	else if (isParticle_) {
		particle_->Draw();
	}

}

void Enemy::Move()
{

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

}

void Enemy::MoveX()
{
	if (moveX_) {

		if (position_.x < moveMaxX_) {
			position_.x += moveSpeed_;
		}
		else {
			moveX_ = false;
		}

	}
	else {

		if (position_.x > -moveMaxX_) {
			position_.x -= moveSpeed_;
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
			position_.y += moveSpeed_;
		}
		else {
			moveY_ = false;
		}

	}
	else {

		if (position_.y > -moveMaxY_) {
			position_.y -= moveSpeed_;
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

void Enemy::InitializeParticle()
{
	//フラグをtrueに
	isParticle_ = true;
	//タイマーセット
	particleTimer_ = ParticleTime_;

	//パーティクル生成
	particle_ = new ParticleManager();
	particle_->Initialize("Resources/effect/effect1.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Zすべてpositionから[+1.0f,-1.0f]でランダムに分布

		XMFLOAT3 pos{};
		pos.x = Random(position_.x - 1.0f, position_.x + 1.0f);
		pos.y = Random(position_.y - 1.0f, position_.y + 1.0f);
		pos.z = Random(position_.z - 1.0f, position_.z + 1.0f);

		//X,Y,Zすべて[-0.05f,+0.05f]でランダムに分布
		const float md_vel = 0.05f;
		XMFLOAT3 vel{};
		vel.x = Random(-md_vel, md_vel);
		vel.y = Random(-md_vel, md_vel);
		vel.z = Random(-md_vel, md_vel);

		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		XMFLOAT3 acc{};
		const float md_acc = -0.001f;
		acc.y = Random(md_acc, 0);

		//追加
		particle_->Add(ParticleTime_, pos, vel, acc);

	}

	particle_->Update();
}

void Enemy::UpdateParticle()
{

	//particle有効時間が過ぎたらフラグをfalseに
	if (particleTimer_ > 0) {
		particleTimer_--;
	}
	else {
		isParticle_ = false;
	}

	if (isParticle_) {
		particle_->Update();
	}
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
	Vector3 velocity = {};

	switch (shotType_)
	{
	case NOTSHOT:

		break;

	case STRAIGHTSHOT:
		//z軸の-方向の単位ベクトルに速度をかける
		velocity = { 0.0f,0.0f,-1.0f };
		velocity *= bulletSpeed_;
		break;

	case HOMINGSHOT:

		//自機と敵のベクトルを取る
		Vector3 playerVec_ = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
		Vector3 enemyVec = { position_.x,position_.y,position_.z };

		velocity = playerVec_ - enemyVec;

		//正規化をして速度をかける
		velocity.normalize();
		velocity *= bulletSpeed_;

		velocity.z += playerSpeed_;

		break;
	}

	//弾の生成
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity, playerSpeed_);
	bullets_.push_back(std::move(newBullet));
}

CollisionData Enemy::GetColData()
{

	CollisionData colData;

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Enemy::SetType(int type)
{

	this->type_ = type;

	switch (type)
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

CollisionData Enemy::GetBulletColData(int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

void Enemy::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}

