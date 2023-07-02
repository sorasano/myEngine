#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#pragma once
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

	void Move();

	bool Collition(XMFLOAT3 pos, XMFLOAT3 size);

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

	bool isDead = false;
};

