/**
* @file Player.cpp
* @brief プレイヤーの処理
*/

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"
#include "PlayerBullet.h"
#include "Sprite.h"

#include "MathManager.h"

#pragma once
class Player
{

public:
	/**
	* シングルトンインスタンス
	*/
	Player* GetInstance();
	/**
	* コンストラクタ
	*/
	Player();
	/**
	* デストラクタ
	*/
	~Player();
	/**
	* 初期化
	*
	*/
	void Initialize();
	/**
	* 更新
	*/
	void Update(XMMATRIX matVP);
	/**
	* 行列更新
	*/
	void UpdateMatrix();
	/**
	* クリアシーン更新
	*/
	void UpdateClearScene();
	/**
	* ゲームオーバーシーン更新
	*/
	void UpdateGameoverScene();
	/**
	* 描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);
	/**
	* クリアシーン描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void DrawClearScene(ID3D12GraphicsCommandList* cmdList);
	/**
	* スプライト描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void DrawSprite(ID3D12GraphicsCommandList* cmdList);


	/**
	* スピードアップ
	*/
	void SpeedUpByEnemy();
	/**
	* スピードダウン
	*/
	void SpeedDownByEnemy();
	/**
	* 移動操作
	*/
	void Move();


	/**
	* 弾発射
	*/
	void Shot();
	/**
	* 弾更新
	*/
	void BulletUpdate();
	/**
	* 弾生成
	*/
	void MakeBullet();


	/**
	* レティクル更新
	*/
	void UpdateRaticle(XMMATRIX matVP);
	/**
	* スプライト更新
	*/
	void UpdateSprite();
	/**
	* リセット
	*/
	void Reset();


	//ゲッター　セッター　
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition() { return position_; }
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotation() { return rotation_; }
	/**
	* @return XMFLOAT3 scale_取得
	*/
	XMFLOAT3 GetScale() { return scale_; }
	/**
	* @return float スピード取得
	*/
	float GetSpeed() { return speedZ_ + addSpeed_; }
	/**
	* @return float 加速スピード取得
	*/
	float GetAddSpeed() { return addSpeed_; }
	/**
	* @return CollisionData 当たり判定情報取得
	*/
	CollisionData GetColData();
	/**
	* @return int 弾の数取得
	*/
	int GetBulletSize() { return static_cast<int>(bullets_.size()); }
	/**
	* @return CollisionData 弾当たり判定情報取得
	*/
	CollisionData GetBulletColData(int i);
	/**
	* @return XMFLOAT3 弾の座標取得
	*/
	XMFLOAT3 GetBulletPosition(int i);

	/**
	* posセット
	*/
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	/**
	* rotセット
	*/
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	/**
	* scaセット
	*/
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }
	/**
	/**
	* スピードセット(基礎)
	*/
	void SetSpeed(float speed) { this->speedZ_ = speed; }
	/**
	* isInvincibleセット
	*/
	void SetIsInvincible(bool isInvincible) { this->isInvincible_ = isInvincible; }
	/**
	* isLockOperation_セット
	*/
	void SetIsLockOperation(bool isLockOperation) { this->isLockOperation_ = isLockOperation; };

	/**
	* 弾死亡情報セット
	*
	* @param[in] isDead 死亡フラグ
	* @param[in] i 何番の弾か
	*/
	void SetBulletIsDead(bool isDead, int i);
private:
	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 1.0f,1.0f,1.0f };

	//2D座標
	XMFLOAT2 position2D_ = {0,0};

private:
	//FBX
	FbxObject3D* playerObject_ = nullptr;
	//モデル
	FbxModel* playerModel_ = nullptr;
	FbxModel* playerBulletModel_ = nullptr;

	//入力
	Input* input_ = nullptr;

	//操作ロック
	bool isLockOperation_ = false;

	//移動範囲
	const XMFLOAT2 MoveMax_ = { 20.0f,10.0f };
	//移動スピード(xy)
	float speedXY_ = 0.5f;
	//移動スピード(z)
	float speedZ_ = 0.5f;

	//基礎スピード以外の加速スピード
	float addSpeed_ = 0.0f;
	//敵を倒した時の加速量
	float addSpeedByEnemy_ = 0.5f;
	//敵に被弾した時の減速
	float subSpeedByEnemy_ = 0.5f;

	//最大加速量
	const float MaxSpeed_ = 5.0f;

	//無敵時間
	bool isInvincible_ = false;
	const int InvincibleTime_ = 10;
	int invincibleTimer_ = 0;

	//--------レティクル--------

	//レティクルスプライト
	std::vector<Sprite*> reticleSprites_;
	//レティクルスプライト数
	int reticleSpriteSize_ = 3;
	//一番手前のサイズ
	XMFLOAT2 reticleScale_ = { 64,64 };

	//レティクル座標(2D)
	XMFLOAT2 reticle2DPosition_ = { 0,0 };
	//レティクル座標(3D)
	Vector3 reticle3DPosition_ = { 0,0,0 };

	//レティクルまでの距離(カメラからプレイヤーの距離+プレイヤーからレティクルの距離)
	const float reticleDirection_ = 30.0f + 26.0f;
	
	//-----スプライト------
	Sprite* speedSprite_;

	//スケールはxが変動、yが固定
	XMFLOAT2 speedSpriteScale_ = { 0,32 };
	XMFLOAT2 speedSpritePosition_ = { window_width / 2,window_height - 64 };

	//スプライトの左右両端をどのくらい開けるか
	float speedSpriteXSpace_ = 64;

private:

	//弾
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	//弾の発射クールタイム
	const int BulletCoolTime_ = 5;
	int bulletCoolTimer_ = 0;

	//弾の速度
	float bulletSpeed_ = 2;

	//初期情報(リセット用)
	XMFLOAT3 initPosition_ = { 0,0,0 };
	XMFLOAT3 initRotation_ = { 0,0,0 };
	XMFLOAT3 initScale_ = { 1,1,1 };

	//基礎スピード
	float initSpeedZ_ = 0.5f;
	//追加スピード
	float initAddSpeed_ = 0.0f;

};

