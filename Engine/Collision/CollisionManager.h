#pragma once
#include "SceneCommonData.h"
#include "Collision.h"

class CollisionManager final
{
public:
	/**
	* シングルトンインスタンスを取得
	*/
	static CollisionManager* GetInstance();

	/**
	* コピーコンストラクタの無効
	*/
	CollisionManager(const CollisionManager& obj) = delete;

	/**
	* 代入演算子の無効
	*/
	CollisionManager& operator=(const CollisionManager& obj) = delete;

public:

	/**
	* コンストラクタ
	*/
	CollisionManager() = default;
	/**
	* デストラクタ
	*/
	~CollisionManager() = default;

public:

	/**
	* 初期化
	*/
	void Initialize(SceneCommonData* cData);
	/**
	* 更新
	*/
	void Update();

	/**
	* 敵と自機の弾の当たり判定
	*/
	void EnemyToPlayerBullet();
	/**
	* 敵と自機の当たり判定
	*/
	void EnemyToPlayer();
	/**
	* 敵と敵
	*/
	void EnemyToEnemy();
	/**
	* 自機と敵の弾
	*/
	void PlayerToEnemyBullet();

	/**
	* ボスと自機の弾の弾
	*/
	void BossToPlayerBullet();
	/**
	* 自機とボスの弾
	*/
	void PlayerToBossBullet();

	/**
	* メニューUI判定
	*
	* @return true = UIが押された
	*/
	bool MenuUIColision();

	/**
	* レティクル座標がどの敵2D座標に一番近いか
	*/
	void ReticleToEnemy2D();

	/**
	* レティクル座標がボス2D座標に当たっているか
	*/
	void ReticleToBoss2D();
private:
	//共通データ
	SceneCommonData* cData_;
};

