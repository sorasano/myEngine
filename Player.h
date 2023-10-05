#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"
#include "PlayerBullet.h"
#include "Sprite.h"

#pragma once
class Player
{
public:
	//シングルトンインスタンス
	Player* GetInstance();
	Player();
	~Player();
	void Initialize(Input* input);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void DrawSprite(ID3D12GraphicsCommandList* cmdList);

	//スピードアップ
	void SpeedUpByEnemy();
	//スピードダウン
	void SpeedDownByEnemy();

	//移動操作
	void Move();

	//弾
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//レティクル
	void UpdateRaticle();
	void MoveRaticle();

	//スピードUI
	void UpdateSprite();

	//リセット
	void Reset();

	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; }
	XMFLOAT3 GetRotation() { return rotation_; }
	XMFLOAT3 GetScale() { return scale_; }
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	float GetSpeed() { return speedZ + addSpeed; }
	CollisionData GetColData();

	void SetIsInvincible(bool isInvincible) { this->isInvincible = isInvincible; }

	//弾
	int GetBulletSize() { return static_cast<int>(bullets_.size()); }
	CollisionData GetBulletColData(int i);
	void SetBulletIsDead(bool isDead,int i);

private:
	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld = {};

	//当たり判定用
	XMFLOAT3 colSize_ = {1.0f,1.0f,1.0f};

private:
	//FBX
	FbxObject3D* playerObject = nullptr;
	//モデル
	FbxModel* playerModel = nullptr;
	FbxModel* playerBulletModel = nullptr;
	FbxModel* fReticleModel = nullptr;
	FbxModel* bReticleModel = nullptr;

	//入力
	Input* input_ = nullptr;

	//移動範囲
	const XMFLOAT2 MoveMax = { 20.0f,10.0f };
	//移動スピード(xy)
	float speedXY = 0.5f;
	//移動スピード(z)
	float speedZ = 0.5f;

	//基礎スピード以外の加速スピード
	float addSpeed = 2.0f;
	//敵を倒した時の加速量
	float addSpeedByEnemy = 0.5f;
	//敵に被弾した時の減速
	float subSpeedByEnemy = 0.5f;

	//最大加速量
	const float MaxSpeed = 5.0f;

	//無敵時間
	bool isInvincible = false;
	const int InvincibleTime = 10;
	int invincibleTimer = 0;

	//--------レティクル--------

	//レティクル座標
	XMFLOAT3 reticlePosition_ = { 0,0,0 };
	//移動範囲
	const XMFLOAT2 ReticleMoveMax = { 40.0f,15.0f };
	//移動スピード(xy)
	float reticleSpeedXY = 1.0f;

	//前(奥)のレティクル
	FbxObject3D* frontReticleObject = nullptr;

	//後ろ(手前)のレティクル
	FbxObject3D* backReticleObject = nullptr;

	//自機からの距離
	const float kDistancePlayerTo3DFrontReticle = 25.0f;
	const float kDistancePlayerTo3DBackReticle = kDistancePlayerTo3DFrontReticle / 2;

	XMFLOAT3 fRPosition_ = { 0,0,0 };
	XMFLOAT3 fRRotation_ = { 0,0,0 };
	XMFLOAT3 fRScale_ = { 1,1,1 };


	XMFLOAT3 bRPosition_ = { 0,0,0 };
	XMFLOAT3 bRRotation_ = { 0,0,0 };
	XMFLOAT3 bRScale_ = { 1,1,1 };

	//ベクトル
	Vector3 playerVec = {};
	Vector3 reticleVec = {};
	Vector3 playerToReticleVec = {};

	//-----スプライト------
	Sprite* speedSprite = nullptr;

	//スケールはxが変動、yが固定
	XMFLOAT2 speedSpriteScale = {0,32};
	XMFLOAT2 speedSpritePosition = { window_width / 2,window_height - 64};

	//スプライトの左右両端をどのくらい開けるか
	float speedSpriteXSpace = 64;

public:

	//弾
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	//弾の発射クールタイム
	const int BulletCoolTime = 5;
	int bulletCoolTimer = 0;

	//弾の速度
	float bulletSpeed = 0;
};

