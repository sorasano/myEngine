#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete test1Sprite;
	delete test2Sprite;
	delete particle1;
	delete particle2;
	FBX_SAFE_DELETE(playerModel);
	FBX_SAFE_DELETE(playerBulletModel);
	for (int i = 0; i < enemySize; i++)
	{
		FBX_SAFE_DELETE(enemyModel);
	}
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input_)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input_;

	//描画初期化処理　ここから

	//カメラ
	//カメラ初期化
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon->GetDevice());
	camera_->Initialize(eye, target, up, input_);

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera_);
	//グラフィックスパイプライン生成
	FbxObject3D::CreateGraphicsPipeline();

	//----------FBX----------
	//モデル名を指定してファイル読み込み
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	enemyModel = FbxLoader::GetInstance()->LoadModelFromFile("enemy");

	//------------プレイヤー----------

	//プレイヤー初期化
	Player* newPlayer = new Player();
	newPlayer->Initialize(input_, playerModel, playerBulletModel);
	player_.reset(newPlayer);

	//------------敵----------
	enemyCsv = new CSVLoader;
	enemyCsv->LoadCSV("Resources/csv/enemy.csv");

	float lowInterval = 50.0f;
	float oldPos = 0.0f;

	for (int i = 0; i < enemySize; i++)
	{
		//CSV
		//std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		//newObject->Initialize(enemyModel);

		//newObject->SetPosition(enemyCsv->GetPosition(i));
		//newObject->SetScale(enemyCsv->GetScale(i));
		//newObject->SetRotation(enemyCsv->GetRotation(i));

		//enemys_.push_back(std::move(newObject));

		//RAND
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(enemyModel);

		//ランダムに分布
		XMFLOAT3 md_pos = { 20.0f,10.0f,3.0f };
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos.x - md_pos.x / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos.y - md_pos.y / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos.z - md_pos.z / 2.0f;

		pos.z = (oldPos + lowInterval);
		oldPos = pos.z;

		newObject->SetPosition(pos);
		enemys_.push_back(std::move(newObject));

	}

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
	//自機
	player_->Update();

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		enemy->Update();
	}

	//当たり判定
	Collition();

	//----パーティクル----
	particle1->Update();
	particle2->Update();

	//カメラ更新
	camera_->Update(player_->GetPosition());

}

void GameScene::Draw()
{

	////-------背景スプライト描画処理-------//
	SpriteManager::GetInstance()->beginDraw();

	//自機
	player_->Draw(dxCommon_->GetCommandList());
	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}


	////-------前景スプライト描画処理-------//
	SpriteManager::GetInstance()->beginDraw();

	//----パーティクル----
	//particle1->Draw();
	//particle2->Draw();

	//スプライト
	//test1Sprite->Draw();
	//test2Sprite->Draw();

}

bool GameScene::UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -500.0f) { return true; }

	return false;
}

void GameScene::Collition()
{
	//敵と自機の弾の当たり判定
	if (player_->GetBulletSize() != 0) {

		for (int i = 0; i < player_->GetBulletSize(); i++) {

			for (std::unique_ptr<Enemy>& enemy : enemys_)
			{
				if (!enemy->GetIsDead()) {
					//当たったか
					if (enemy->Collition(player_->GetBulletPosition(i), player_->GetBulletColSize(i))) {

						//当たったら自機の弾を消し、自機のスピードを上げスコアを加算
						player_->SetBulletIsDead(true, i);
						player_->SpeedUpByEnemy();

					}
				}
			}
		}
	}

	//敵と自機の当たり判定
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (!enemy->GetIsDead()) {
			//当たったか
			if (enemy->Collition(player_->GetPosition(), player_->GetColSize())) {

				//当たったら自機のスピードを下げ,少し無敵時間に
				player_->SpeedDownByEnemy();
				player_->SetIsInvincible(true);
			}
		}
	}

}
