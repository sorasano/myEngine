#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "EnemyBullet.h"

enum EnemyType {
	NOTHING,
	NORMAL,
	HOMING,
	MOVINGX,
	MOVINGY,
	MOVINGREF,
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
	MOVEREF
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

	//当たり判定
	bool Collition(XMFLOAT3 pos, XMFLOAT3 size);

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

	void SetISDesd(bool isDead) { this->isDead = isDead; }
	bool GetIsDead() { return isDead; }
	bool GetIsParticle() { return isParticle; }
private:
	//アフィン変換情報;
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld = {};

private:
	//FBX
	FbxObject3D* enemyObject = nullptr;

	//敵種類	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int type = 0;
	//射撃タイプ 0 何もしない 1 直線 2 プレイヤーに向かって
	int shotType = 0;
	//移動タイプ 0 移動しない 1 X軸 2 Y軸 3反射移動
	int moveType = 0;

	//画面内で停滞するか
	bool stopInScreen = false;

	//死亡フラグ
	bool isDead = false;


	//----------弾----------
	FbxModel* enemyBulletModel_ = nullptr;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullet_;

	//弾の発射クールタイム
	const int BulletCoolTime = 50;
	int bulletCoolTimer = 0;

	//弾の速度
	float bulletSpeed = 1.0f;

	//弾を打ち始める範囲,プレイヤーから何進んだ距離からか
	const float ShotStart = 100.0f;
	float shotStartPos = 100.0f;


	//-----パーティクル-----
	ParticleManager* particle = nullptr;
	bool isParticle = false;

	//パーティクル時間
	const int ParticleTime = 30;
	int particleTimer = 0;
	
	//------プレイヤー------
	XMFLOAT3 playerPosition_ = {};
	float playerSpeed_ = 0.0f;
};

