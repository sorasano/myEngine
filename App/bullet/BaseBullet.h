﻿/**
* @file BaseBullet.h
* @brief 弾の基本機能
*/

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "Collision.h"
#include "MathManager.h"

#pragma once
class BaseBullet
{
public:
	/**
	* コンストラクタ
	*/
	BaseBullet();

	/**
	* デストラクタ
	*/
	virtual ~BaseBullet() = default;

	/**
	* 更新
	*/
	void Update();

	/**
	* 敵,ボス更新
	*
	* @param[in] deletePos カメラZ座標
	*/
	void EnemyUpdate(float cameraZ);

	/**
	* 描画
	*
	* @param[out] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);


private:

	/**
	* 移動
	*/
	void Move();
	/**
	* 当たった時の挙動
	*/
	void OnCollision();

public:

	//ゲッター　セッター　
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition()const { return position_; };
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotation() const { return rotation_; };
	/**
	* @return XMFLOAT3 _AAAAAscale_
	*/
	XMFLOAT3 GetScale() const { return scale_; };
	/**
	* @return bool isDead_取得
	*/
	bool GetIsDead()const { return isDead_; }
	/**
	* @return CollisionData 当たり判定情報取得
	*/
	CollisionData GetColData()const;

	/**
	* position_セット
	*/
	void SetPosition(const XMFLOAT3& pos) { this->position_ = pos; }
	/**
	* rotation_セット
	*/
	void SetRotation(const XMFLOAT3& rot) { this->rotation_ = rot; }
	/**
	* scale_セット
	*/
	void SetScale(const XMFLOAT3& sca) { this->scale_ = sca; }
	/**
	* isDead_セット
	*/
	void SetIsDead(bool isDead) { this->isDead_ = isDead; };

protected:
	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1.5f,1.5f,1.5f };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 0.5f,0.5f,2.0f };

protected:
	//FBX
	FbxObject3D* bulletObject_ = nullptr;

	//移動ベクトル
	Vector3 velocity_ = {};

	//寿命
	static const int32_t lifeTime_ = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = lifeTime_;
	//デスフラグ
	bool isDead_ = false;

};

