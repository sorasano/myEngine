#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Camera.h"
#include "object3D.h"
#include "Model.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "CSVLoader.h"

#include "Player.h"
#include "Enemy.h"

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

	void Collition();

	//敵生存確認
	void CheckEnemy();

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
	FbxModel* playerModel = nullptr;
	FbxModel* playerBulletModel = nullptr;
	FbxModel* enemyModel = nullptr;
	FbxModel* fReticleModel = nullptr;
	FbxModel* bReticleModel = nullptr;


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

	//ビュー変換行列
	XMFLOAT3 eye = { 0, 1, 30 };
	XMFLOAT3 target = { 0, 0, 0 };
	XMFLOAT3 up = { 0, 1, 0 };

};

