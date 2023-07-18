#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"
#include "PlayerBullet.h"

#pragma once
class Player
{
public:
	//シングルトンインスタンス
	Player* GetInstance();
	Player();
	~Player();
	void Initialize(Input* input, FbxModel* playerModel, FbxModel* playerBulletModel,FbxModel* fRModel, FbxModel* bRModel);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//スピードアップ
	void SpeedUpByEnemy();
	//スピードダウン
	void SpeedDownByEnemy();

	//弾
	void Move();
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//レティクル
	void UpdateRaticle();

	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; }
	XMFLOAT3 GetRotation() { return rotation_; }
	XMFLOAT3 GetScale() { return scale_; }
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	XMFLOAT3 GetColSize(){ return colSize; }
	//弾
	int GetBulletSize() { return static_cast<int>(playerBullet_.size()); }
	XMFLOAT3 GetBulletPosition(int i);
	XMFLOAT3 GetBulletRotation(int i);
	XMFLOAT3 GetBulletScale(int i);
	XMFLOAT3 GetBulletColSize(int i);

	void SetBulletIsDead(bool isDead,int i);
	void SetIsInvincible(bool isInvincible){ this->isInvincible = isInvincible; }

private:
	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld = {};

	//当たり判定用
	XMFLOAT3 colSize = {1.0f,1.0f,1.0f};

private:
	//FBX
	FbxObject3D* playerObject = nullptr;
	//入力
	Input* input_ = nullptr;

	//移動範囲
	const XMFLOAT2 MoveMax = { 20.0f,10.0f };
	//移動スピード(xy)
	float speedXY = 0.5f;
	//移動スピード(z)
	float speedZ = 1.0f;

	//基礎スピード以外の加速スピード
	float addSpeed = 0.0f;
	//敵を倒した時の加速量
	float addSpeedByEnemy = 1.5f;
	//敵に被弾した時の減速
	float subSpeedByEnemy = 1.5f;

	//最大加速量
	const float MaxSpeed = 15.0f;

	//無敵時間
	bool isInvincible = false;
	const int InvincibleTime = 10;
	int invincibleTimer = 0;

	//レティクル
	FbxObject3D* frontReticleObject = nullptr;
	FbxObject3D* backReticleObject = nullptr;

	//自機からの距離
	const float kDistancePlayerTo3DFrontReticle = 50.0f;
	const float kDistancePlayerTo3DBackReticle = 25.0f;

	XMFLOAT3 fRPosition_ = { 0,0,0 };
	XMFLOAT3 fRRotation_ = { 0,0,0 };
	XMFLOAT3 fRScale_ = { 1,1,1 };

	XMFLOAT3 bRPosition_ = { 0,0,0 };
	XMFLOAT3 bRRotation_ = { 0,0,0 };
	XMFLOAT3 bRScale_ = { 1,1,1 };

public:

	//弾
	FbxModel* playerBulletModel_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> playerBullet_;

	//弾の発射クールタイム
	const int BulletCoolTime = 10;
	int bulletCoolTimer = 0;

};

