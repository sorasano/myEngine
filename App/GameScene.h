/**
* @file GameScene.h
* @brief ゲームシーン
*/

#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Scene.h"
#include "Camera.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "CSVLoader.h"
#include "JsonLoader.h"
#include "Collision.h"
#include "PerformanceManager.h"

#include "Skydome.h"
#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"

#include <map>

class GameScene
{
public:
	/**
	* コンストラクタ
	*/
	GameScene();
	/**
	* デストラクタ
	*/
	~GameScene();
	/**
	* 初期化	
	*
	*/
	void Initialize();
	/**
	* 更新
	*/
	void Update();
	/**
	* 描画
	*/
	void Draw();
	/**
	* スプライト描画
	*/
	void DrawSprite();


	/**
	* 更新範囲
	*
	* @param[in] cameraPos カメラ座標
	* @param[in] pos 対象座標
	* @return bool 更新範囲内ならtrue
	*/
	bool UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos);
	/**
	* 当たり判定
	*/
	void Collition();
	/**
	* 敵生存確認
	*/
	void CheckEnemy();
	/**
	* 背景更新
	*/
	void UpdateBackGround();
	/**
	* 敵配置
	*/
	void SetEnemy();
	/**
	* リセット
	*/
	void Reset();
	/**
	* シーン切り替え
	*/
	void ChangeScene();
	/**
	* プレイシーン初期化
	*/
	void PlaySceneInitialize();
	/**
	* ボス戦初期化
	*/
	void BossSceneInitialize();
	/**
	* ボス戦当たり判定
	*/
	void BossSceneCollition();

private:
	//デバイスとinput
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//カメラ
	Camera* camera_{};

	//シーン
	int scene_ = TITLE;

	//フェーズ
	int phase_ = 0;
	const int MaxPhase_ = 10;

	//Fbx
	FbxModel* enemyModel_ = nullptr;
	FbxModel* enemyBulletModel_ = nullptr;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_;

	//背景
	std::list<std::unique_ptr<BackGround>> backGrounds_;
	size_t backGroundSize_ = 5;
	//調整用の座標
	float adjustPos_ = 0;

	//プレイヤー
	std::unique_ptr<Player> player_;

	//敵
	std::list<std::unique_ptr<Enemy>> enemys_;

	//csv
	std::list<std::unique_ptr<CSVLoader>> enemyCsvs_;
	std::vector<std::string> enemyCsvsName_;
	int enemyCSVSize_ = 0;

	//ボス
	std::unique_ptr<Boss> boss_;

	//スプライトマネージャー
	SpriteManager* spriteManager_ = nullptr;
	//スプライト
	Sprite* titleSprite_ = nullptr;
	Sprite* clearSprite_ = nullptr;
	Sprite* gameoverSprite_ = nullptr;

	//当たり判定
	Collision* collisionManager_ = nullptr;

	//演出
	PerformanceManager* performanceManager_ = nullptr;

	//-----パーティクル-----
	ParticleManager* particleManager_ = nullptr;

};

