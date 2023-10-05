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
	FBX_SAFE_DELETE(BossObject);
}

void Boss::Initialize()
{
	//モデル名を指定してファイル読み込み
	normalBossModel = FbxLoader::GetInstance()->LoadModelFromFile("normalBoss");
	hardBossModel = FbxLoader::GetInstance()->LoadModelFromFile("hardBoss");

	bossBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("bossBullet");

	//3dオブジェクト生成とモデルのセット
	BossObject = new FbxObject3D;
	BossObject->Initialize();
	BossObject->SetModel(normalBossModel);

	this->bulletModel_ = bossBulletModel;

}

void Boss::Update(XMFLOAT3 pPos, float pSpeed)
{
	//プレイヤー情報更新
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead) {

		//行動変化
		actionCollTimer++;
		if (actionCollTimer > ActionCoolTime) {
			actionCollTimer = 0;
			ChangeAction();
		}

		//移動
		Move();

		//射撃
		if (shotType != BOSSNOTSHOT) {
			//プレイヤーのスピードで発射し始める座標を変更
			shotStartPos = ShotStart * playerSpeed_;
			if (position_.z < playerPosition_.z + shotStartPos) {
				BulletUpdate();
			}
		}

	}

	//パーティクル
	if (isParticle) {
		UpdateParticle();
	}

	BossObject->SetPosition(position_);
	BossObject->SetScale(scale_);
	BossObject->SetRotate(rotation_);
	BossObject->Update();
}

void Boss::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead) {
		BossObject->Draw(cmdList);

		//弾
		for (std::unique_ptr<BossBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}
	//----パーティクル----
	if (isParticle) {
		particle->Draw();
	}

}

void Boss::Move()
{
	//自機に追従
	position_.z += playerSpeed_;

	switch (moveType)
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
	if (moveX) {

		if (position_.x < moveMaxX) {
			position_.x += moveSpeed;
		}
		else {
			moveX = false;
		}

	}
	else {

		if (position_.x > -moveMaxX) {
			position_.x -= moveSpeed;
		}
		else {
			moveX = true;
		}
	}
}

void Boss::MoveY()
{
	if (moveY) {

		if (position_.y < moveMaxY) {
			position_.y += moveSpeed;
		}
		else {
			moveY = false;
		}

	}
	else {

		if (position_.y > -moveMaxY) {
			position_.y -= moveSpeed;
		}
		else {
			moveY = true;
		}
	}
}


void Boss::InitializeParticle()
{
	//フラグをtrueに
	isParticle = true;
	//タイマーセット
	particleTimer = ParticleTime;

	//パーティクル生成
	particle = new ParticleManager();
	particle->Initialize("Resources/effect/effect1.png");

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

void Boss::UpdateParticle()
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

void Boss::Shot()
{

	//クールタイムで弾の発射間隔を調整
	bulletCoolTimer++;

	if (BulletCoolTime < bulletCoolTimer) {

		//射出角度を計算し弾の発射
		Vector3 velocity = {};
		Vector3 playerVec = {};
		Vector3 BossVec = {};

		switch (shotType)
		{
		case BOSSNOTSHOT:

			break;

		case BOSSSTRAIGHTSHOT:
			//z軸の-方向の単位ベクトルに速度をかける
			velocity = { 0.0f,0.0f,-1.0f };
			velocity *= bulletSpeed;

			//弾の生成
			MakeBullet(velocity);
			break;

		case BOSSHOMINGSHOT:

			//自機と敵のベクトルを取る
			playerVec = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
			BossVec = { position_.x,position_.y,position_.z };

			velocity = playerVec - BossVec;

			//正規化をして速度をかける
			velocity.normalize();
			velocity *= bulletSpeed;

			velocity.z += playerSpeed_;

			//弾の生成
			MakeBullet(velocity);
			break;

		case BOSSMULTISHOT:

			//正面
			velocity = { 0.0f,0.1f,-1.0f };
			velocity *= bulletSpeed;
			//弾の生成
			MakeBullet(velocity);

			//上
			velocity = { 0.0f,0.1f,-1.0f };
			velocity *= bulletSpeed;
			//弾の生成
			MakeBullet(velocity);

			//下
			velocity = { 0.0f,-0.1f,-1.0f };
			velocity *= bulletSpeed;
			//弾の生成
			MakeBullet(velocity);

			//右
			velocity = { 0.1f,0.0f,-1.0f };
			velocity *= bulletSpeed;
			//弾の生成
			MakeBullet(velocity);

			//左
			velocity = { -0.1f,0.0f,-1.0f };
			velocity *= bulletSpeed;
			//弾の生成
			MakeBullet(velocity);

			break;
		}

		bulletCoolTimer = 0;
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
	isDead = false;

	//2段階目移行フラグ
	isBossHardMode = false;

	//hp
	hp = 10;

	particleTimer = 0;
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
	if (hp) {
		hp--;
	}
	else {
		isDead = true;
	}

	//hpが2段階目移行タイミングになったら2段階目へ
	if (hp <= changeHardHp) {
		isBossHardMode = true;
		BossObject->SetModel(hardBossModel);
	}
	 
	 
}

void Boss::SetNormalAction(int action)
{

	this->normalAction = action;

	switch (action)
	{
	case BOSSNOTHING:
		shotType = BOSSNOTSHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSNORMAL:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSHOMING:
		shotType = BOSSHOMINGSHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSMOVINGX:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSMOVEX;
		break;

	case BOSSMOVINGY:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSMOVEY;
		break;

	case BOSSMOVINGDIA:
		shotType = BOSSHOMINGSHOT;
		moveType = BOSSMOVEDIA;
		break;

	}
}

void Boss::SetHardAction(int action)
{
	this->normalAction = action;

	switch (action)
	{
	case BOSSHARDNOTHING:
		shotType = BOSSMULTISHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSHARDNORMAL:
		shotType = BOSSMULTISHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSHARDHOMING:
		shotType = BOSSMULTISHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSHARDMOVINGX:
		shotType = BOSSMULTISHOT;
		moveType = BOSSMOVEX;
		break;

	case BOSSHARDMOVINGY:
		shotType = BOSSMULTISHOT;
		moveType = BOSSMOVEY;
		break;

	case BOSSHARDMOVINGDIA:
		shotType = BOSSMULTISHOT;
		moveType = BOSSMOVEDIA;
		break;

	}
}

void Boss::ChangeAction()
{

	//通常モード
	if (!isBossHardMode) {
		int randNum = static_cast<int>(Random(0, normalActionSize - 0.001f));
		SetNormalAction(randNum);
	}
	else {
	//2段階目モード
		int randNum = static_cast<int>(Random(0, hardActionSize - 0.001f));
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

