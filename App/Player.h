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
	void Update(const XMMATRIX& matVP);
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
	void UpdateRaticle(const XMMATRIX& matVP);
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
	XMFLOAT3 GetPosition() const { return position_; }
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotation()const { return rotation_; }
	/**
	* @return XMFLOAT3 scale_取得
	*/
	XMFLOAT3 GetScale()const { return scale_; }
	/**
	* @return float スピード取得
	*/
	float GetSpeed() const { return speedZ_ + mainAddSpeed_; }
	/**
	* @return float 加速スピード取得
	*/
	float GetAddSpeed() const { return mainAddSpeed_; }
	/**
	* @return CollisionData 当たり判定情報取得
	*/
	CollisionData GetColData()const;
	/**
	* @return int 弾の数取得
	*/
	int GetBulletSize()const { return static_cast<int>(bullets_.size()); }
	/**
	* @return CollisionData 弾当たり判定情報取得
	*/
	CollisionData GetBulletColData(int i)const;
	/**
	* @return XMFLOAT3 弾の座標取得
	*/
	XMFLOAT3 GetBulletPosition(int i)const;

	/**
	* posセット
	*/
	void SetPosition(const XMFLOAT3& pos) { this->position_ = pos; }
	/**
	* rotセット
	*/
	void SetRotation(const XMFLOAT3& rot) { this->rotation_ = rot; }
	/**
	* scaセット
	*/
	void SetScale(const XMFLOAT3& sca) { this->scale_ = sca; }
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
	XMFLOAT2 position2D_ = { 0,0 };

private:
	//FBX
	std::unique_ptr<FbxObject3D> playerObject_ = nullptr;
	//モデル
	std::unique_ptr<FbxModel> playerModel_ = nullptr;
	std::unique_ptr<FbxModel> bulletModel_ = nullptr;

	//入力
	MyEngine::Input* input_ = nullptr;

	//操作ロック
	bool isLockOperation_ = false;

	//移動範囲
	const XMFLOAT2 MoveMax_ = { 20.0f,10.0f };
	//移動スピード(xy)
	float speedXY_ = 0.5f;
	//移動スピード(z)
	float speedZ_ = 0.5f;

	//(メイン)
	//基礎スピード以外の加速スピード
	float mainAddSpeed_ = 0.0f;
	//レベルアップしたときの加速量
	float mainAddSpeedBySub_ = 1.0f;
	//レベルダウンしたときの減速量
	float mainSubSpeedBySub_ = 1.0f;
	//最大加速量
	const float MainMaxSpeed_ = 5.0f;

	//(サブ)
	//基礎スピード以外の加速スピード
	float subAddSpeed_ = 0.0f;
	//敵を倒した時の加速量
	float subAddSpeedByEnemy_ = 0.5f;
	//敵に被弾した時の減速
	float subSubSpeedByEnemy_ = 1.0f;
	//最大加速量
	const float SubMaxSpeed_ = 5.0f;

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
	std::unique_ptr<Sprite> mainSpeedSprite_;
	std::unique_ptr<Sprite> subSpeedSprite_;

	//スケールはxが変動、yが固定
	XMFLOAT2 mainSpeedSpriteScale_ = { 0,32 };
	XMFLOAT2 mainSpeedSpritePosition_ = { window_width / 2,window_height - 64 };
	XMFLOAT2 subSpeedSpriteScale_ = { 0,32 };
	XMFLOAT2 subSpeedSpritePosition_ = { window_width / 2,window_height - 32 };

	//スプライトの左右両端をどのくらい開けるか
	float speedSpriteXSpace_ = 64;

private:

	//弾
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
	//追加スピード(メイン)
	float initMainAddSpeed_ = 0.0f;
	//追加スピード(サブ)
	float initSubAddSpeed_ = 0.0f;

};