#pragma once
/**
* @file BaseScene.h
* @brief シーンの基底クラス
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
#include "Menu.h"

#include "Skydome.h"
#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"

#include <map>

//前方宣言
class SceneManager;

class BaseScene {

public:

	BaseScene();

	/**
	* デストラクタ
	*/
	virtual ~BaseScene() = default;

	/**
	* 初期化
	*
	*/
	virtual void Initialize() = 0;
	/**
	* 更新
	*/
	virtual void Update() = 0;
	/**
	* 描画
	*/
	virtual void Draw() = 0;
	/**
	* スプライト描画
	*/
	virtual void DrawSprite() = 0;
	/**
	* 終了処理
	*/
	virtual void Finalize() = 0;

	//セッター
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

	//共通処理

	/**
	* 更新
	*/
	void CommonUpdate();
	/**
	* 描画
	*/
	void CommonDraw();
	/**
	* スプライト描画
	*/
	void CommonDrawSprite();

	/**
	* 更新範囲
	*
	* @param[in] cameraPos カメラ座標
	* @param[in] pos 対象座標
	* @return bool 更新範囲内ならtrue
	*/
	bool UpadateRange(const XMFLOAT3& cameraPos, const XMFLOAT3& pos);

	/**
	* 背景更新
	*/
	void UpdateBackGround();

protected:

	//シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;

	//デバイスとinput
	MyEngine::DirectXCommon* dxCommon_ = nullptr;
	MyEngine::Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	//当たり判定
	Collision* collisionManager_ = nullptr;

	//カメラ
	std::unique_ptr<Camera> camera_{};

	//演出
	std::unique_ptr<PerformanceManager> performanceManager_ = nullptr;

	//パーティクル
	std::unique_ptr<ParticleManager> particleManager_ = nullptr;

	//メニュー
	std::unique_ptr<Menu> menu_;
	//プレイヤー
	std::unique_ptr<Player> player_;

	//敵
	std::list<std::unique_ptr<Enemy>> enemys_;

	//ボス
	std::unique_ptr<Boss> boss_;

	//フェーズ
	int phase_ = 0;
	const int MaxPhase_ = 3;

	//-----リソース-----

	//Fbx
	std::unique_ptr<FbxModel> enemyModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyBlueModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyYellowModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyBulletModel_ = nullptr;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_;
	//背景
	std::list<std::unique_ptr<BackGround>> backGrounds_;
	size_t backGroundSize_ = 5;
	//調整用の座標
	float adjustPos_ = 0;

	//csv
	std::list<std::unique_ptr<CSVLoader>> enemyCsvs_;
	std::vector<std::string> enemyCsvsName_;
	int enemyCSVSize_ = 0;

	//スプライトマネージャー
	std::unique_ptr <SpriteManager> spriteManager_ = nullptr;
	//スプライト
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	std::unique_ptr<Sprite> clearSprite_ = nullptr;
	std::unique_ptr<Sprite> gameoverSprite_ = nullptr;
	std::unique_ptr<Sprite> menuUISprite_ = nullptr;

};

