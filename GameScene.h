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

#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"

#include <map>

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize(DirectXCommon* dxCommon, Input* input);

	void Update();

	void Draw();

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

private:
	//デバイスとinput
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//カメラ
	Camera* camera_{};

	//Fbx
	FbxModel* enemyModel = nullptr;
	FbxModel* enemyBulletModel = nullptr;

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

	//スプライトマネージャー
	SpriteManager* spriteManager = nullptr;
	//テクスチャ
	Sprite* testSprite = nullptr;

	//当たり判定
	Collision* collisionManager_ = nullptr;

	//描画最大距離
	float rangeMaxZ = 500.0f;
};

