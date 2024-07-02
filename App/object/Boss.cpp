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
}

void Boss::Initialize()
{
	//モデル名を指定してファイル読み込み
	normalBossModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("normalBoss"));
	hardBossModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("hardBoss"));
	bulletModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("bossBullet"));
	shadowModel_.reset(FbxLoader::GetInstance()->LoadShadowModelFromFile("normalBoss"));

	//3dオブジェクト生成とモデルのセット
	std::unique_ptr<FbxObject3D> newBossObject_ = std::make_unique<FbxObject3D>();
	newBossObject_->Initialize();
	newBossObject_->SetModel(normalBossModel_.get());
	bossObject_.swap(newBossObject_);

	//モデルの角度調整
	this->rotation_.x = static_cast<float>(90 * (PI / 180));
	this->rotation_.y = static_cast<float>(270 * (PI / 180));

	Reset();

	//影
	std::unique_ptr<Shadow> newShadow = std::make_unique<Shadow>();
	newShadow->Initialize(shadowModel_.get());
	shadow_.swap(newShadow);

}

void Boss::Update(const XMFLOAT3& pPos, float pSpeed, float cameraZ)
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
				BulletUpdate(cameraZ);
			}
		}

	}

	//影更新
	shadow_->Update(position_, rotation_);

	//行列更新
	UpdateMatrix();
}

void Boss::UpdateMatrix()
{
	bossObject_->SetPosition(position_);
	bossObject_->SetScale(scale_);
	bossObject_->SetRotate(rotation_);
	bossObject_->Update();
}

void Boss::UpdateClearScene()
{
	UpdateMatrix();
}

void Boss::UpdateGameoverScene()
{
	//移動
	const float speed = -1.0f;
	position_.z += speed;

	//影更新
	shadow_->Update(position_, rotation_);
	UpdateMatrix();

	//弾があったら削除
	if (bullets_.size()) {
		bullets_.clear();
	}
}

void Boss::Draw(ID3D12GraphicsCommandList* cmdList)
{

	bossObject_->Draw(cmdList);

	if (!isDead_) {
		//弾
		for (std::unique_ptr<BossBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}

	//影
	shadow_->Draw(cmdList);
}

void Boss::Move()
{
	//ベクトルのリセット
	moveVec_.zero();

	//自機に追従
	moveVec_.z = +playerSpeed_;

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

	//ベクトルの加算
	VecAddXMFLOAT3(position_, moveVec_);
}

void Boss::MoveX()
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
			moveX_ = true;
		}
	}
}

void Boss::MoveY()
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
			BossVec = { position_.x,position_.y,position_.z + playerSpeed_ };

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

void Boss::BulletUpdate(float cameraZ)
{
	//プレイヤーより前にいる敵の弾のみ発射
	if (position_.z > playerPosition_.z) {
		Shot();
	}

	//敵の弾更新
	for (std::unique_ptr<BossBullet>& bullet : bullets_)
	{
		bullet->EnemyUpdate(cameraZ);
	}

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<BossBullet>& bullet) {return bullet->GetIsDead(); });
}

void Boss::MakeBullet(const Vector3& velocity)
{
	//弾の生成
	std::unique_ptr<BossBullet> newBullet = std::make_unique<BossBullet>();
	newBullet->Initialize(bulletModel_.get(), position_, velocity, playerSpeed_);
	bullets_.push_back(std::move(newBullet));

}

void Boss::Reset()
{
	position_ = initPosition_;

	//弾
	bullets_.clear();

	//死亡フラグ
	isDead_ = false;

	//2段階目移行フラグ
	isBossHardMode_ = false;
	bossObject_->SetModel(normalBossModel_.get());

	//hp
	hp_ = initHp_;

}

CollisionData Boss::GetColData()
{

	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Boss::HitBullet()
{
	//hpがあるなら減らす、ないなら死亡
	if (hp_ > 0) {
		hp_--;
	}
	else {
		isDead_ = true;
	}

	//hpが2段階目移行タイミングになったら2段階目へ
	if (hp_ <= changeHardHp_) {
		isBossHardMode_ = true;
		bossObject_->SetModel(hardBossModel_.get());
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

CollisionData Boss::GetBulletColData(int i) const
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

XMFLOAT3 Boss::GetBulletPosition(int i) const
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetPosition();
}

void Boss::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}

