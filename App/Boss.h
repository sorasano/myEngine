#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "BossBullet.h"

#include "Collision.h"

enum BossNormalAction {
	BOSSNOTHING,
	BOSSNORMAL,
	BOSSHOMING,
	BOSSMOVINGX,
	BOSSMOVINGY,
	BOSSMOVINGDIA,
};

enum BossHardAction {
	BOSSHARDNOTHING,
	BOSSHARDNORMAL,
	BOSSHARDHOMING,
	BOSSHARDMOVINGX,
	BOSSHARDMOVINGY,
	BOSSHARDMOVINGDIA,
};

enum BossShotType {
	BOSSNOTSHOT,
	BOSSSTRAIGHTSHOT,
	BOSSHOMINGSHOT,
	BOSSMULTISHOT
};

enum BossMoveType {
	BOSSNOTMOVE,
	BOSSMOVEX,
	BOSSMOVEY,
	BOSSMOVEDIA
};

class Boss
{
public:
	//シングルトンインスタンス
	Boss* GetInstance();
	Boss();
	~Boss();
	void Initialize();
	void Update(XMFLOAT3 pPos, float pSpeed);
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//移動
	void Move();
	void MoveX();
	void MoveY();

	//パーティクル
	void InitializeParticle();
	void UpdateParticle();

	//弾
	void Shot();
	void BulletUpdate();
	void MakeBullet(Vector3 velocity);

	//リセット
	void Reset();

	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	void SetISDesd(bool isDead) { this->isDead_ = isDead; }
	bool GetIsDead() { return isDead_; }
	void SetIsParticle(bool isParticle) { this->isParticle_ = isParticle; }
	bool GetIsParticle() { return isParticle_; }

	void SetNormalAction(int action);
	void SetHardAction(int action);

	void ChangeAction();

	//当たり判定
	CollisionData GetColData();

	//自機の弾との当たり判定
	void HitBullet();

	//弾
	int GetBulletSize() { return static_cast<int>(bullets_.size()); }
	CollisionData GetBulletColData(int i);
	void SetBulletIsDead(bool isDead, int i);

private:
	//アフィン変換情報;
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 2.0f,2.0f,1.0f };
private:
	//FBX
	FbxObject3D* BossObject_ = nullptr;
	//モデル
	FbxModel* normalBossModel_ = nullptr;
	FbxModel* hardBossModel_ = nullptr;

	FbxModel* bossBulletModel_ = nullptr;

	//死亡フラグ
	bool isDead_ = false;

	//通常行動	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int normalAction_ = 0;
	//2段階目行動	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int hardAction_ = 0;
	//射撃タイプ 0 何もしない 1 直線 2 プレイヤーに向かって
	int shotType_ = 0;
	//移動タイプ 0 移動しない 1 X軸 2 Y軸 3斜め
	int moveType_ = 0;

	//アクション変化
	const int ActionCoolTime_ = 100;
	int actionCollTimer_ = 0;
	//アクション数(通常)
	int normalActionSize_ = 6;
	//アクション数(2段階目)
	int hardActionSize_ = 6;

	//ボス2段階目フラグ
	bool isBossHardMode_ = 0;
	//hp
	int hp_ = 50;
	//2段階目になるhp
	int changeHardHp_ = 25;

	//-------移動-------
	//移動 trueが+に移動中でfalseが-に移動中
	bool moveX_ = true;
	bool moveY_ = true;
	//移動制限
	float moveMaxX_ = 10;
	float moveMaxY_ = 10;
	//移動スピード
	float moveSpeed_ = 0.1f;

	//----------弾----------
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<BossBullet>> bullets_;

	//弾の発射クールタイム
	const int BulletCoolTime_ = 50;
	int bulletCoolTimer_ = 0;

	//弾の速度
	float bulletSpeed_ = 1.0f;

	//弾を打ち始める範囲,プレイヤーから何進んだ距離からか
	const float ShotStart_ = 100.0f;
	float shotStartPos_ = 100.0f;


	//-----パーティクル-----
	ParticleManager* particle_ = nullptr;
	bool isParticle_ = false;

	//パーティクル時間
	const int ParticleTime_ = 30;
	int particleTimer_ = 0;

	//------プレイヤー------
	XMFLOAT3 playerPosition_ = {};
	float playerSpeed_ = 0.0f;
};

