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
	MOVING,
};

class Enemy
{
public:
	//シングルトンインスタンス
	Enemy* GetInstance();
	Enemy();
	~Enemy();
	void Initialize(FbxModel* EnemyModel);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void UpdateNothing();
	void UpdateNormal();
	void UpdateHoming();
	void UpdateMoving();

	bool Collition(XMFLOAT3 pos, XMFLOAT3 size);

	void InitializeParticle();
	void UpdateParticle();

	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	bool GetIsDead() { return isDead; }
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

	//死亡フラグ
	bool isDead = false;

	//0 = Nothing　何もしない,1 = normal 正面にたまを打つだけ 2 = homing　ホーミング弾を打つ, 3 = 動きながら正面に弾を打つ
	int type = 0;
	//画面内で停滞するか
	bool stopInScreen = false;

	//----------弾----------
	FbxModel* enemyBulletModel_ = nullptr;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullet_;

	//弾の発射クールタイム
	const int BulletCoolTime = 10;
	int bulletCoolTimer = 0;

	//-----パーティクル-----
	ParticleManager* particle = nullptr;
	bool isParticle = false;

	//パーティクル時間
	const int ParticleTime = 30;
	int particleTimer = 0;

};

