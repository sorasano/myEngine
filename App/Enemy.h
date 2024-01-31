/**
* @file Enemy.h
* @brief 敵の処理
*/

#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "EnemyBullet.h"

#include "Collision.h"

enum EnemyType {
	NOTHING,
	NORMAL,
	HOMING,
	MOVINGX,
	MOVINGY,
	MOVINGDIA,
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
	MOVEDIA
};

class Enemy
{
public:
	/**
	* シングルトンインスタンス
	*/
	Enemy* GetInstance();
	/**
	* コンストラクタ
	*/
	Enemy();
	/**
	* デストラクタ
	*/
	~Enemy();
	/**
	* 初期化
	*
	* @param[in] EnemyModel 敵FBXモデル
	* @param[in] enemyBulletModel 弾FBXモデル
	*/
	void Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel);
	/**
	* 更新
	*
	* @param[in] pPos プレイヤー座標
	* @param[in] pSpeed プレイヤースピード
	*/
	void Update(const XMFLOAT3& pPos,float pSpeed);
	/**
	* 描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);


	/**
	* 移動
	*/
	void Move();
	/**
	* X移動
	*/
	void MoveX();
	/**
	* Y移動
	*/
	void MoveY();
	/**
	* 移動反射
	*/
	void Reflection();

	/**
	* 画面内に停滞させる
	*/
	void StopInScreen();

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
	* @return XMFLOAT3 scale_取得
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
	* @return int 弾の数取得
	*/
	int GetBulletSize()const { return static_cast<int>(bullets_.size()); }
	/**
	* @return CollisionData 弾当たり判定情報取得
	*/
	CollisionData GetBulletColData(int i) const;


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
	* isDeadセット
	*/
	void SetISDesd(bool isDead) { this->isDead_ = isDead; }
	/**
	* typeセット
	*/
	void SetType(int type);
	/**
	* stopInScreenセット
	*/
	void SetStopInScreen(bool stopInScreen) { this->stopInScreen_ = stopInScreen; }
	/**
	* 弾死亡情報セット
	*
	* @param[in] isDead 死亡フラグ
	* @param[in] i 何番の弾か
	*/
	void SetBulletIsDead(bool isDead, int i);

private:
	//アフィン変換情報;
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 2.0f,2.0f,1.0f };
private:
	//FBX
	std::unique_ptr<FbxObject3D> enemyObject_ = nullptr;

	//敵種類	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int type_ = 0;
	//射撃タイプ 0 何もしない 1 直線 2 プレイヤーに向かって
	int shotType_ = 0;
	//移動タイプ 0 移動しない 1 X軸 2 Y軸 3斜め
	int moveType_ = 0;

	//死亡フラグ
	bool isDead_ = false;

	//画面内で停滞するか
	bool stopInScreen_ = false;
	//画面内で停滞しているか
	bool isStopInScreen_ = false;
	//何秒停滞するか
	const int StopInScreenTime_ = 600;
	int stopInScreenTimer_ = 0;
	//プレイヤーからどの位置に停止するか
	float stopInScreenPosition_ = 26.0f;

	//-------移動-------
	//移動 trueが+に移動中でfalseが-に移動中
	bool moveX_ = true;
	bool moveY_ = true;
	//移動制限
	float moveMaxX_ = 10;
	float moveMaxY_ = 10;
	//移動スピード
	float moveSpeed_ = 0.1f;

	//----------弾----------
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	//弾の発射クールタイム
	const int BulletCoolTime_ = 50;
	int bulletCoolTimer_ = 0;

	//弾の速度
	float bulletSpeed_ = 1.0f;

	//弾を打ち始める範囲,プレイヤーから何進んだ距離からか
	const float ShotStart_ = 100.0f;
	float shotStartPos_ = 100.0f;

	//------プレイヤー------
	XMFLOAT3 playerPosition_ = {};
	float playerSpeed_ = 0.0f;
};

