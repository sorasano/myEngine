#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Camera.h"
#include "object3D.h"
#include "Model.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "Collision.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"

#include "Player.h"
#include "Sphere.h"
#include "Plane.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize(DirectXCommon* dxCommon, Input* input);

	void Update();

	void Draw();

private:
	//デバイスとinput
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	//カメラ
	std::unique_ptr<Camera> camera_;

	ID3D12GraphicsCommandList* commandList = nullptr;

	//オブジェクト
	std::unique_ptr<Object3D> object3Ds_;
	//球オブジェクト
	std::unique_ptr<Model> sphereModel_;
	std::unique_ptr<Sphere> sphere_;

	std::unique_ptr<Model> sphereRedModel_;
	std::unique_ptr<Sphere> sphereRed_;

	//Fbx
	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObject1 = nullptr;

	FbxModel* fbxModel2 = nullptr;
	FbxObject3D* fbxObject2 = nullptr;

	//カメラ初期化
	Camera* camera{};

	//----スプライト----

	//テクスチャ
	uint32_t test1Texture = 0;		
	Sprite* test1Sprite = nullptr;

	uint32_t test2Texture = 0;	
	Sprite* test2Sprite = nullptr;

	//パーティクル
	ParticleManager* particle1 = nullptr;
	ParticleManager* particle2 = nullptr;

	//アフィン変換情報
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,1,0 };

	//ビュー変換行列

	XMFLOAT3 eye = { -10, 1, 50 };
	XMFLOAT3 target = { 0, 0, 0 };
	XMFLOAT3 up = { 0, 1, 0 };

	//当たり判定
	bool hit = false;

};

