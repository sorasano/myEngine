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
	FBX_SAFE_DELETE(enemyObject);
}

void Enemy::Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel)
{

	//3dオブジェクト生成とモデルのセット
	enemyObject = new FbxObject3D;
	enemyObject->Initialize();
	enemyObject->SetModel(EnemyModel);

	this->enemyBulletModel_ = enemyBulletModel;

	type = HOMING;

	switch (type)
	{
	case NOTHING:
		shotType = NOTSHOT;
		moveType = NOTMOVE;
		break;

	case NORMAL:
		shotType = STRAIGHTSHOT;
		moveType = NOTMOVE;
		break;

	case HOMING:
		shotType = HOMINGSHOT;
		moveType = NOTMOVE;
		break;

	case MOVINGX:
		shotType = STRAIGHTSHOT;
		moveType = MOVEX;
		break;

	case MOVINGY:
		shotType = STRAIGHTSHOT;
		moveType = MOVEY;
		break;

	case MOVINGREF:
		shotType = STRAIGHTSHOT;
		moveType = MOVEREF;
		break;
	}
}

void Enemy::Update(XMFLOAT3 pPos, float pSpeed)
{
	//プレイヤー情報更新
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead) {

		//射撃
		if (shotType != NOTSHOT) {
			//プレイヤーのスピードで発射し始める座標を変更
			shotStartPos = ShotStart * playerSpeed_;

			if (position_.z < playerPosition_.z + shotStartPos) {
				BulletUpdate();
			}
		}

		//移動
		if (moveType != NOTMOVE) {
			Move();
		}

	}
	//パーティクル
	else if (isParticle) {
		UpdateParticle();
	}

	enemyObject->SetPosition(position_);
	enemyObject->SetScale(scale_);
	enemyObject->SetRotate(rotation_);
	enemyObject->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead) {
		enemyObject->Draw(cmdList);

		//弾
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullet_)
		{
			bullet->Draw(cmdList);
		}
	}
	//----パーティクル----
	else if (isParticle) {
		particle->Draw();
	}

}

void Enemy::Move()
{

}

void Enemy::InitializeParticle()
{
	//フラグをtrueに
	isParticle = true;
	//タイマーセット
	particleTimer = ParticleTime;

	//パーティクル生成
	particle = new ParticleManager();
	particle->Initialize("Resources/effect1.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Zすべてpositionから[+1.0f,-1.0f]でランダムに分布
		const float md_pos = 1.0f;

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
		particle->Add(ParticleTime, pos, vel, acc);

	}

	particle->Update();
}

void Enemy::UpdateParticle()
{

	//particle有効時間が過ぎたらフラグをfalseに
	if (particleTimer > 0) {
		particleTimer--;
	}
	else {
		isParticle = false;
	}

	if (isParticle) {
		particle->Update();
	}
}

void Enemy::Shot()
{
	bulletCoolTimer++;

	if (BulletCoolTime < bulletCoolTimer) {
		MakeBullet();

		bulletCoolTimer = 0;
	}

}

void Enemy::BulletUpdate()
{
	//プレイヤーより前にいる敵の弾のみ発射
	if (position_.z > playerPosition_.z) {
		Shot();
	}

	//敵の弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullet_)
	{
		bullet->Update();
	}

	//デスフラグの立った弾を削除
	enemyBullet_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {return bullet->GetIsDead(); });
}

void Enemy::MakeBullet()
{
	Vector3 velocity = {};

	switch (shotType)
	{
	case NOTSHOT:

		break;

	case STRAIGHTSHOT:
		//z軸の-方向の単位ベクトルに速度をかける
		velocity = { 0.0f,0.0f,-1.0f };
		velocity *= bulletSpeed;
		break;

	case HOMINGSHOT:

		//自機と敵のベクトルを取る
		Vector3 playerVec = { playerPosition_.x,playerPosition_.y,playerPosition_.z };
		Vector3 enemyVec = { position_.x,position_.y,position_.z };

		velocity = playerVec - enemyVec;

		//正規化をして速度をかける
		velocity.normalize();
		velocity *= bulletSpeed;

		break;
	}

	//弾の生成
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(enemyBulletModel_, position_, velocity);
	enemyBullet_.push_back(std::move(newBullet));
}

bool Enemy::Collition(XMFLOAT3 pos, XMFLOAT3 size)
{
	XMFLOAT3 ePos = position_;
	XMFLOAT3 eSize = scale_;

	float ePosX1 = ePos.x - eSize.x;
	float ePosX2 = ePos.x + eSize.x;

	float ePosY1 = ePos.y - eSize.y;
	float ePosY2 = ePos.y + eSize.y;

	float ePosZ1 = ePos.z - eSize.z;
	float ePosZ2 = ePos.z + eSize.z;

	XMFLOAT3 bPos = pos;
	XMFLOAT3 bSize = size;

	float bPosX1 = bPos.x - bSize.x;
	float bPosX2 = bPos.x + bSize.x;

	float bPosY1 = bPos.y - bSize.y;
	float bPosY2 = bPos.y + bSize.y;

	float bPosZ1 = bPos.z - bSize.z;
	float bPosZ2 = bPos.z + bSize.z;


	if (ePosX1 < bPosX2 && bPosX1 < ePosX2) {
		if (ePosY1 < bPosY2 && bPosY1 < ePosY2) {
			if (ePosZ1 < bPosZ2 && bPosZ1 < ePosZ2) {

				//自機の弾に当たったら死亡
				isDead = true;

				//パーティクル生成
				InitializeParticle();
			}
		}
	}

	return isDead;
}
