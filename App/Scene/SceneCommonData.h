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
#include "PerformanceManager.h"
#include "Menu.h"

#include "Skydome.h"
#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"

#include <map>

class SceneCommonData
{

public:
	SceneCommonData();
	
public:

	//デバイスとinput
	MyEngine::DirectXCommon* dxCommon_ = nullptr;
	MyEngine::Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//カメラ
	std::unique_ptr<Camera> camera_{};

	//演出
	std::unique_ptr<PerformanceManager> performanceManager_ = nullptr;

	//パーティクル
	std::unique_ptr<ParticleManager> destroyParticle_ = nullptr;
	std::unique_ptr<ParticleManager> landingParticle_ = nullptr;

	//メニュー
	std::unique_ptr<Menu> menu_;
	//プレイヤー
	std::unique_ptr<Player> player_;

	//敵
	std::vector<std::unique_ptr<Enemy>> enemys_;

	//ボス
	std::unique_ptr<Boss> boss_;

	//シーン
	int scene_ = TITLE;

	//フェーズ
	int phase_ = 0;
	const int MaxPhase_ = 1;

	//-----リソース-----

	//Fbx
	std::unique_ptr<FbxModel> enemyModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyBlueModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyYellowModel_ = nullptr;

	std::unique_ptr<FbxModel> enemyShadowModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyBlueShadowModel_ = nullptr;
	std::unique_ptr<FbxModel> enemyYellowShadowModel_ = nullptr;

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

