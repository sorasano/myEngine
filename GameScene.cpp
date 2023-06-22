#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete test1Sprite;
	delete test2Sprite;
	delete camera;
	delete particle1;
	delete particle2;
	FBX_SAFE_DELETE(fbxModel1);
	FBX_SAFE_DELETE(fbxObject1);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input_)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input_;

	//描画初期化処理　ここから
	HRESULT result;

	//カメラ
	//カメラ初期化
	camera = new Camera;
	camera->StaticInitialize(dxCommon->GetDevice());
	camera->Initialize(eye, target, up, input_);

	camera->SetTarget({0,0,0});
	camera->SetEye({ 0,0,30 });

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera);
	//グラフィックスパイプライン生成
	FbxObject3D::CreateGraphicsPipeline();

	//モデル名を指定してファイル読み込み
	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("cube");

	//3dオブジェクト生成とモデルのセット
	fbxObject1 = new FbxObject3D;
	fbxObject1->Initialize();
	fbxObject1->SetModel(fbxModel1);

	//モデル名を指定してファイル読み込み
	fbxModel2 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//3dオブジェクト生成とモデルのセット
	fbxObject2 = new FbxObject3D;
	fbxObject2->Initialize();
	fbxObject2->SetModel(fbxModel2);
	fbxObject2->PlayAnimation();

	//3dモデル
	//----------球----------
	//球オブジェクトのモデル初期化
	Model* newSphereModel = new Model();
	newSphereModel->Initialize(dxCommon, "sphere", "Resources/sphere/white1×1.png");
	sphereModel_.reset(newSphereModel);

	//球オブジェクト初期化
	Sphere* newSphere = new Sphere();
	newSphere->Initialize(dxCommon, sphereModel_.get());
	sphere_.reset(newSphere);

	//球オブジェクトのモデル初期化
	Model* newSphereRedModel = new Model();
	newSphereRedModel->Initialize(dxCommon, "sphere", "Resources/sphere/red1x1.png");
	sphereRedModel_.reset(newSphereRedModel);

	//球オブジェクト初期化
	Sphere* newRedSphere = new Sphere();
	newRedSphere->Initialize(dxCommon, sphereRedModel_.get());
	sphereRed_.reset(newRedSphere);

	//---------地面-----------

	////地面オブジェクト
	//std::unique_ptr<Model> planeModel_;
	//std::unique_ptr<Plane> plane_;

	////地面オブジェクトのモデル初期化
	//Model* newPlaneModel = new Model();
	//newPlaneModel->Initialize(dxCommon, "plane", "Resources/plane/white1×1.png");
	//planeModel_.reset(newPlaneModel);

	////地面オブジェクト初期化
	//Plane* newPlane = new Plane();
	//newPlane->Initialize(dxCommon,planeModel_.get());
	//plane_.reset(newPlane);


	//------------プレイヤー----------

	//プレイヤーのモデル
	//std::unique_ptr<Model> playerModel_;

	////プレイヤー
	//std::unique_ptr<Player> player_;

	////プレイヤーのモデル初期化
	//Model* newModel = new Model();
	//newModel->Initialize(dxCommon, "Player", "Resources/Player/blue.png");
	//playerModel_.reset(newModel);

	////プレイヤー初期化
	//Player* newPlayer = new Player();
	//newPlayer->Initialize(dxCommon, playerModel_.get());
	//player_.reset(newPlayer);

	//当たり判定

	////球
	sphere_->sphereCol.radius = 1;
	sphere_->sphereCol.center = XMVECTOR{ sphere_->GetPosition().x, sphere_->GetPosition().y, sphere_->GetPosition().z,1 };

	//スプライト生成
	//---test1---
	test1Texture = Texture::LoadTexture(L"Resources/reimu.png");

	test1Sprite = new Sprite();
	test1Sprite->Initialize(test1Texture);
	//アンカーポイントをスプライトの中心に
	test1Sprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	test1Sprite->SetPos(XMFLOAT2(WinApp::winW / 2, WinApp::winH / 2));
	test1Sprite->Update();

	//---test2---
	test2Texture = Texture::LoadTexture(L"Resources/texture.jpg");

	test2Sprite = new Sprite();
	test2Sprite->Initialize(test2Texture);
	//アンカーポイントをスプライトの中心に
	test2Sprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	test2Sprite->SetPos(XMFLOAT2(WinApp::winW / 2, WinApp::winH / 2 - 200));
	test2Sprite->Update();

	//パーティクル1

	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon, WinApp::winW, WinApp::winH);


	particle1 = new ParticleManager();
	//パーティクル生成
	particle1->Initialize("Resources/effect1.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Zすべて[-5.0f,+5.0f]でランダムに分布
		const float md_pos = 10.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		//X,Y,Zすべて[-0.05f,+0.05f]でランダムに分布
		const float md_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		XMFLOAT3 acc{};
		const float md_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		//追加
		particle1->Add(600, pos, vel, acc);

	}

	particle1->Update();


	particle2 = new ParticleManager();
	//パーティクル生成
	particle2->Initialize("Resources/effect2.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Zすべて[-5.0f,+5.0f]でランダムに分布
		const float md_pos = 10.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		//X,Y,Zすべて[-0.05f,+0.05f]でランダムに分布
		const float md_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		XMFLOAT3 acc{};
		const float md_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		//追加
		particle2->Add(600, pos, vel, acc);

	}

	particle2->Update();
}

void GameScene::Update()
{
	//射影変換

	//カメラ更新
	camera->Update();

	//----球----

	//平行移動更新

	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//座標を移動する処理(Z座標)
		if (input_->PushKey(DIK_UP)) { position_.y += 0.1f; }
		else if (input_->PushKey(DIK_DOWN)) { position_.y -= 0.1f; }
		if (input_->PushKey(DIK_RIGHT)) { position_.x -= 0.1f; }
		else if (input_->PushKey(DIK_LEFT)) { position_.x += 0.1f; }

	}

	sphere_->setPosition(position_);
	sphere_->setRotation(XMFLOAT3(0, 0, 0));
	/*	sphere_->setScale(XMFLOAT3(1, 100, 100));*/

	sphere_->Update(camera->matView, camera->matProjection);

	sphereRed_->setPosition(XMFLOAT3(position_.x + 3, position_.y, position_.z));
	sphereRed_->Update(camera->matView, camera->matProjection);

	//plane_->Update(matView, matProjection);

	sphere_->sphereCol.center = XMVECTOR{ sphere_->GetPosition().x, sphere_->GetPosition().y, sphere_->GetPosition().z,1 };

	//hit = Collision::CheckSphere2Plane(sphere_->sphereCol,plane_->planeCol);


	//fbx
	fbxObject1->Update();
	fbxObject2->Update();

	//----パーティクル----
	particle1->Update();
	particle2->Update();

}

void GameScene::Draw()
{

	////-------背景スプライト描画処理-------//
	SpriteManager::GetInstance()->beginDraw();

	//sphere_->Draw();
	//sphereRed_->Draw();

	//fbxObject1->Draw(dxCommon_->GetCommandList());
	fbxObject2->Draw(dxCommon_->GetCommandList());


	////-------前景スプライト描画処理-------//
	SpriteManager::GetInstance()->beginDraw();

	//----パーティクル----
	//particle1->Draw();
	//particle2->Draw();

	//スプライト
	//test1Sprite->Draw();
	//test2Sprite->Draw();

}
