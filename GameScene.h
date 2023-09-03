#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Camera.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "CSVLoader.h"
#include "JsonLoader.h"
#include "Collision.h"

#include "Skydome.h"
#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"

#include <map>

enum Scene {
	TITLE,
	NORMALPLAY,
	BOSSPLAY,
	CLEAR
};

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize(DirectXCommon* dxCommon, Input* input);

	void Update();

	void Draw();

	void DrawSprite();

	//更新範囲
	bool UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos);

	//当たり判定
	void Collition();

	//敵生存確認
	void CheckEnemy();

	//背景更新
	void UpdateBackGround();

	//敵配置
	void SetEnemy();

	//リセット
	void Reset();

	//ボス戦
	//導入演出,初期化
	void BossSceneInitialize();
	//撃破演出
	void BossSceneFinalize();

	void BossSceneCollition();


private:
	//デバイスとinput
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//カメラ
	Camera* camera_{};

	//シーン
	int scene = TITLE;

	//フェーズ
	int phase = 0;
	const int MaxPhase = 1;

	//Fbx
	FbxModel* enemyModel = nullptr;
	FbxModel* enemyBulletModel = nullptr;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_;

	//背景
	std::list<std::unique_ptr<BackGround>> backGrounds_;
	size_t backGroundSize = 4;
	//調整用の座標
	float adjustPos = 0;

	//プレイヤー
	std::unique_ptr<Player> player_;

	//敵
	std::list<std::unique_ptr<Enemy>> enemys_;

	//csv
	std::list<std::unique_ptr<CSVLoader>> enemyCsvs_;
	std::vector<std::string> enemyCsvsName_;
	int enemyCSVSize = 0;

	//ボス
	std::unique_ptr<Boss> boss_;

	//スプライトマネージャー
	SpriteManager* spriteManager = nullptr;
	//スプライト
	Sprite* titleSprite = nullptr;
	Sprite* clearSprite = nullptr;


	//当たり判定
	Collision* collisionManager_ = nullptr;

	//描画最大距離
	float rangeMaxZ = 500.0f;
};

