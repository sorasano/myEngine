/**
* @file Boss.h
* @brief ボスの処理
*/

#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "Collision.h"
#include "ParticleManager.h"

#include "BossBullet.h"
#include "Shadow.h"

enum BossNormalAction {
	BOSSNOTHING,
	BOSSNORMAL,
	BOSSHOMING,
	BOSSMOVINGX,
	BOSSMOVINGY,
	BOSSMOVINGDIA,
};

enum BossHardAction {
	BOSSHARDNOTHING,
	BOSSHARDNORMAL,
	BOSSHARDHOMING,
	BOSSHARDMOVINGX,
	BOSSHARDMOVINGY,
	BOSSHARDMOVINGDIA,
};

enum BossShotType {
	BOSSNOTSHOT,
	BOSSSTRAIGHTSHOT,
	BOSSHOMINGSHOT,
	BOSSMULTISHOT
};

enum BossMoveType {
	BOSSNOTMOVE,
	BOSSMOVEX,
	BOSSMOVEY,
	BOSSMOVEDIA
};

class Boss
{
public:
	/**
	* シングルトンインスタンス
	*/
	Boss* GetInstance();
	/**
	* コンストラクタ
	*/
	Boss();
	/**
	* デストラクタ
	*/
	~Boss();
	/**
	* 初期化
	*/
	void Initialize();
	/**
	* 更新
	*
	* @param[in] pPos プレイヤー座標
	* @param[in] pSpeed プレイヤースピード
	* @param[in] deletePos カメラZ座標
	*/
	void Update(const XMFLOAT3& pPos, float pSpeed, float cameraZ);

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
	* @param[out] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);
	/**
	* リセット
	*/
	void Reset();

private:

	/**
	* 行列更新
	*/
	void UpdateMatrix();
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
	* 弾発射
	*/
	void Shot();
	/**
	* 弾更新
	* 
	* @param[in] deletePos カメラZ座標
	*/
	void BulletUpdate(float cameraZ);
	/**
	* 弾生成
	*
	* @param[in] velocity 発射ベクトル
	*/
	void MakeBullet(const Vector3& velocity);

public:

	//ゲッター　セッター　
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition() const { return position_; };
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotation()const { return rotation_; };
	/**
	* @return XMFLOAT3 scale_取得
	*/
	XMFLOAT3 GetScale() const { return scale_; };
	/**
	* @return bool isDead_取得
	*/
	bool GetIsDead() const { return isDead_; }
	/**
	* @return int 弾の数取得
	*/
	int GetBulletSize()const { return static_cast<int>(bullets_.size()); }
	/**
	* @return CollisionData 弾当たり判定情報取得
	*/
	CollisionData GetBulletColData(int i) const;
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
	* isDeadセット
	*/
	void SetISDesd(bool isDead) { this->isDead_ = isDead; }
	/**
	* actionセット
	*/
	void SetNormalAction(int action);
	/**
	* actionセット
	*/
	void SetHardAction(int action);
	/**
	* 弾死亡情報セット
	*
	* @param[in] isDead 死亡フラグ
	* @param[in] i 何番の弾か
	*/
	void SetBulletIsDead(bool isDead, int i);


	/**
	* 行動変更
	*/
	void ChangeAction();
	/**
	* 当たり判定
	*/
	CollisionData GetColData();
	/**
	* 自機の弾との当たり判定
	*/
	void HitBullet();

private:

	//変換情報;
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//当たり判定用
	XMFLOAT3 colSize_ = { 2.0f,2.0f,1.0f };
private:
	//FBX
	std::unique_ptr<FbxObject3D> bossObject_ = nullptr;
	//モデル
	std::unique_ptr<FbxModel> normalBossModel_ = nullptr;
	std::unique_ptr<FbxModel> hardBossModel_ = nullptr;
	std::unique_ptr<FbxModel> bulletModel_ = nullptr;
	std::unique_ptr<FbxModel> shadowModel_ = nullptr;

	//死亡フラグ
	bool isDead_ = false;

	//通常行動	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int normalAction_ = 0;
	//2段階目行動	 0 Nothing　何もしない,1 normal 正面にたまを打つだけ 2 homing　ホーミング弾を打つ, 3～5 動きながら正面に弾を打つ
	int hardAction_ = 0;
	//射撃タイプ 0 何もしない 1 直線 2 プレイヤーに向かって
	int shotType_ = 0;
	//移動タイプ 0 移動しない 1 X軸 2 Y軸 3斜め
	int moveType_ = 0;

	//アクション変化
	const int ActionCoolTime_ = 100;
	int actionCollTimer_ = 0;
	//アクション数(通常)
	int normalActionSize_ = 6;
	//アクション数(2段階目)
	int hardActionSize_ = 6;

	//ボス2段階目フラグ
	bool isBossHardMode_ = 0;
	//hp
	int hp_ = 200;
	//hp
	int initHp_ = hp_;

	//2段階目になるhp
	int changeHardHp_ = 100;

	//影
	std::unique_ptr<Shadow> shadow_;

	//-------移動-------
	//移動 trueが+に移動中でfalseが-に移動中
	bool moveX_ = true;
	bool moveY_ = true;
	//移動制限
	float moveMaxX_ = 10;
	float moveMaxY_ = 10;
	//移動スピード
	float moveSpeed_ = 0.1f;
	//移動ベクトル
	Vector3 moveVec_ = {};

	//----------弾----------
	std::list<std::unique_ptr<BossBullet>> bullets_;

	//弾の発射クールタイム
	const int BulletCoolTime_ = 2;
	int bulletCoolTimer_ = 0;

	//弾の速度
	float bulletSpeed_ = 1.0f;

	//弾を打ち始める範囲,プレイヤーから何進んだ距離からか
	const float ShotStart_ = 100.0f;
	float shotStartPos_ = 100.0f;

	//------プレイヤー------
	XMFLOAT3 playerPosition_ = {};
	float playerSpeed_ = 0.0f;

	//初期情報(リセット用)
	XMFLOAT3 initPosition_ = { 0,0,0 };
	XMFLOAT3 initRotation_ = { 0,0,0 };
	XMFLOAT3 initScale_ = { 1,1,1 };
};

