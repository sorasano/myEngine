#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "EnemyBullet.h"

#include "Collision.h"

enum EnemyType {
	NOTHING,
	NORMAL,
	HOMING,
	MOVINGX,
	MOVINGY,
	MOVINGDIA,
};

enum ShotType {
	NOTSHOT,
	STRAIGHTSHOT,
	HOMINGSHOT
};

enum MoveType {
	NOTMOVE,
	MOVEX,
	MOVEY,
	MOVEDIA
};

class Enemy
{
public:
	//シングルトンインスタンス
	Enemy* GetInstance();
	Enemy();
	~Enemy();
	void Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel);
	void Update(XMFLOAT3 pPos,float pSpeed);
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//移動
	void Move();
	void MoveX();
	void MoveY();

	void Reflection();

	//画面内に停滞させる
	void StopInScreen();

	//パーティクル
	void InitializeParticle();
	void UpdateParticle();

	//弾
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	void SetISDesd(bool isDead) { this->isDead_ = isDead; }
	bool GetIsDead() { return isDead_; }
	bool GetIsParticle() { return isParticle_; }

	CollisionData GetColData();

	void SetType(int type);
	void SetStopInScreen(bool stopInScreen) { this->stopInScreen_ = stopInScreen; }

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
	FbxObject3D* enemyObject_ = nullptr;

	//敵種類	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int type_ = 0;
	//射撃タイプ 0 何もしない 1 直線 2 プレイヤーに向かって
	int shotType_ = 0;
	//移動タイプ 0 移動しない 1 X軸 2 Y軸 3斜め
	int moveType_ = 0;

	//死亡フラグ
	bool isDead_ = false;

	//画面内で停滞するか
	bool stopInScreen_ = false;
	//画面内で停滞しているか
	bool isStopInScreen_ = false;
	//何秒停滞するか
	const int StopInScreenTime_ = 600;
	int stopInScreenTimer_ = 0;
	//プレイヤーからどの位置に停止するか
	float stopInScreenPosition_ = 26.0f;

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
	std::list<std::unique_ptr<EnemyBullet>> bullets_;

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

