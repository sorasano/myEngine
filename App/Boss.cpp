/**
* @file Boss.cpp
* @brief ボスの処理
*/

#include "Boss.h"
#include "Random.h"

Boss* Boss::GetInstance()
{
	static Boss instance;
	return &instance;
}

Boss::Boss()
{
}

Boss::~Boss()
{
	FBX_SAFE_DELETE(BossObject_);
}

void Boss::Initialize()
{
	//モデル名を指定してファイル読み込み
	normalBossModel_ = FbxLoader::GetInstance()->LoadModelFromFile("normalBoss");
	hardBossModel_ = FbxLoader::GetInstance()->LoadModelFromFile("hardBoss");

	bossBulletModel_ = FbxLoader::GetInstance()->LoadModelFromFile("bossBullet");

	//3dオブジェクト生成とモデルのセット
	BossObject_ = new FbxObject3D;
	BossObject_->Initialize();
	BossObject_->SetModel(normalBossModel_);

	this->bulletModel_ = bossBulletModel_;

}

void Boss::Update(XMFLOAT3 pPos, float pSpeed)
{
	//プレイヤー情報更新
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead_) {

		//行動変化
		actionCollTimer_++;
		if (actionCollTimer_ > ActionCoolTime_) {
			actionCollTimer_ = 0;
			ChangeAction();

			//行動変化時弾があったら削除
			bullets_.clear();
		}

		//移動
		Move();

		//射撃
		if (shotType_ != BOSSNOTSHOT) {
			//プレイヤーのスピードで発射し始める座標を変更
			shotStartPos_ = ShotStart_ * playerSpeed_;
			if (position_.z < playerPosition_.z + shotStartPos_) {
				BulletUpdate();
			}
		}

	}

	BossObject_->SetPosition(position_);
	BossObject_->SetScale(scale_);
	BossObject_->SetRotate(rotation_);
	BossObject_->Update();
}

void Boss::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead_) {
		BossObject_->Draw(cmdList);

		//弾
		for (std::unique_ptr<BossBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}
}

void Boss::Move()
{
	//自機に追従
	position_.z += playerSpeed_;

	switch (moveType_)
	{
	case BOSSNOTMOVE:
		break;

	case BOSSMOVEX:
		MoveX();
		break;

	case BOSSMOVEY:
		MoveY();
		break;

	case BOSSMOVEDIA:
		MoveX();
		MoveY();
		break;
	}

}

void Boss::MoveX()
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
			moveX_ = true;
		}
	}
}

void Boss::MoveY()
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

void Boss::Shot()
{

	//クールタイムで弾の発射間隔を調整
	bulletCoolTimer_++;

	if (BulletCoolTime_ < bulletCoolTimer_) {

		//射出角度を計算し弾の発射
		Vector3 velocity = {};
		Vector3 playerVec_ = {};
		Vector3 BossVec = {};

		switch (shotType_)
		{
		case BOSSNOTSHOT:

			break;

		case BOSSSTRAIGHTSHOT:
			//z軸の-方向の単位ベクトルに速度をかける
			velocity = { 0.0f,0.0f,-1.0f };
			velocity *= bulletSpeed_;
			velocity.z += playerSpeed_;

			//弾の生成
			MakeBullet(velocity);
			break;

		case BOSSHOMINGSHOT:

			//自機と敵のベクトルを取る
			playerVec_ = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
			BossVec = { position_.x,position_.y,position_.z };

			velocity = playerVec_ - BossVec;

			//正規化をして速度をかける
			velocity.normalize();
			velocity *= bulletSpeed_;

			velocity.z += playerSpeed_;

			//弾の生成
			MakeBullet(velocity);
			break;

		case BOSSMULTISHOT:

			//正面
			velocity = { 0.0f,0.0f,-1.0f };
			velocity *= bulletSpeed_;
			velocity.z += playerSpeed_;
			//弾の生成
			MakeBullet(velocity);

			//上
			velocity = { 0.0f,0.1f,-1.0f };
			velocity *= bulletSpeed_;
			velocity.z += playerSpeed_;
			//弾の生成
			MakeBullet(velocity);

			//下
			velocity = { 0.0f,-0.1f,-1.0f };
			velocity *= bulletSpeed_;
			velocity.z += playerSpeed_;
			//弾の生成
			MakeBullet(velocity);

			//右
			velocity = { 0.1f,0.0f,-1.0f };
			velocity *= bulletSpeed_;
			velocity.z += playerSpeed_;
			//弾の生成
			MakeBullet(velocity);

			//左
			velocity = { -0.1f,0.0f,-1.0f };
			velocity *= bulletSpeed_;
			velocity.z += playerSpeed_;
			//弾の生成
			MakeBullet(velocity);

			break;
		}

		bulletCoolTimer_ = 0;
	}

}

void Boss::BulletUpdate()
{
	//プレイヤーより前にいる敵の弾のみ発射
	if (position_.z > playerPosition_.z) {
		Shot();
	}

	//敵の弾更新
	for (std::unique_ptr<BossBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<BossBullet>& bullet) {return bullet->GetIsDead(); });
}

void Boss::MakeBullet(Vector3 velocity)
{
	//弾の生成
	std::unique_ptr<BossBullet> newBullet = std::make_unique<BossBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity, playerSpeed_);
	bullets_.push_back(std::move(newBullet));

}

void Boss::Reset()
{
	position_ = { 0,0,0 };

	//弾
	bullets_.clear();

	//死亡フラグ
	isDead_ = false;

	//2段階目移行フラグ
	isBossHardMode_ = false;

	//hp
	hp_ = 1;

}

CollisionData Boss::GetColData()
{

	CollisionData colData;

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Boss::HitBullet()
{
	//hpがあるなら減らす、ないなら死亡
	if (hp_) {
		hp_--;
	}
	else {
		isDead_ = true;
	}

	//hpが2段階目移行タイミングになったら2段階目へ
	if (hp_ <= changeHardHp_) {
		isBossHardMode_ = true;
		BossObject_->SetModel(hardBossModel_);
	}
	 
	 
}

void Boss::SetNormalAction(int action)
{

	this->normalAction_ = action;

	switch (action)
	{
	case BOSSNOTHING:
		shotType_ = BOSSNOTSHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSNORMAL:
		shotType_ = BOSSSTRAIGHTSHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHOMING:
		shotType_ = BOSSHOMINGSHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSMOVINGX:
		shotType_ = BOSSSTRAIGHTSHOT;
		moveType_ = BOSSMOVEX;
		break;

	case BOSSMOVINGY:
		shotType_ = BOSSSTRAIGHTSHOT;
		moveType_ = BOSSMOVEY;
		break;

	case BOSSMOVINGDIA:
		shotType_ = BOSSHOMINGSHOT;
		moveType_ = BOSSMOVEDIA;
		break;

	}
}

void Boss::SetHardAction(int action)
{
	this->normalAction_ = action;

	switch (action)
	{
	case BOSSHARDNOTHING:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHARDNORMAL:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHARDHOMING:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHARDMOVINGX:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSMOVEX;
		break;

	case BOSSHARDMOVINGY:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSMOVEY;
		break;

	case BOSSHARDMOVINGDIA:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSMOVEDIA;
		break;

	}
}

void Boss::ChangeAction()
{

	//通常モード
	if (!isBossHardMode_) {
		int randNum = static_cast<int>(Random(0, normalActionSize_ - 0.001f));
		SetNormalAction(randNum);
	}
	else {
	//2段階目モード
		int randNum = static_cast<int>(Random(0, hardActionSize_ - 0.001f));
		SetHardAction(randNum);
	}
}

CollisionData Boss::GetBulletColData(int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

void Boss::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}

