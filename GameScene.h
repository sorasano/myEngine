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
	bool UpadateRange(XMFLOAT3 cameraPos,XMFLOAT3 pos);

	//当たり判定
	void Collition();

	//敵生存確認
	void CheckEnemy();

	//背景更新
	void UpdateBackGround();

private:
	//デバイスとinput
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//カメラ
	Camera* camera_{};

	//csv
	CSVLoader* enemyCsv = nullptr;

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
	size_t enemySize = 100;


	//----スプライト----

	//スプライトマネージャー
	SpriteManager* spriteManager = nullptr;

	//テクスチャ
	Sprite* testSprite = nullptr;

};

