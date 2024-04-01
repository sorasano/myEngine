/**
* @file PlayerBullet.cpp
* @brief プレイヤーの弾の処理
*/

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "Collision.h"
#include "MathManager.h"

#pragma once
class PlayerBullet
{
public:
	/**
	* コンストラクタ
	*/
	PlayerBullet();
	/**
	* デストラクタ
	*/
	~PlayerBullet();
	/**
	* 初期化
	*
	* @param[in] model fbxモデル
	* @param[in] position 発生座標
	* @param[in] velocity 発射ベクトル
	* @param[in] playerSpeed プレイヤーのスピード
	*/
	void Initialize(FbxModel* model, const XMFLOAT3& position, const Vector3& velocity);
	/**
	* 更新
	*/
	void Update();
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

private:
	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 0.5f,0.5f,2.0f };

private:
	//FBX
	FbxObject3D* playerBulletObject_ = nullptr;

	//移動ベクトル
	Vector3 velocity_ = {};

	//寿命
	static const int32_t lifeTime_ = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = lifeTime_;
	//デスフラグ
	bool isDead_ = false;

};

