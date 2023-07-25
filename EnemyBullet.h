#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "Vector3.h"

#pragma once
class EnemyBullet
{
public:
	//シングルトンインスタンス
	EnemyBullet* GetInstance();
	EnemyBullet();
	~EnemyBullet();
	void Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void Move();
	void OnCollision();
	bool GetIsDead()const { return isDead_; }
	void SetIsDead(bool isDead) { this->isDead_ = isDead; };

	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	XMFLOAT3 GetColSize() { return colSize_; };

private:
	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 0.2f,0.2f,1.0f };

private:
	//FBX
	FbxObject3D* EnemyBulletObject = nullptr;

	//移動ベクトル
	Vector3 velocity_ = {};

	//寿命
	static const int32_t lifeTime = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = lifeTime;
	//デスフラグ
	bool isDead_ = false;

};

