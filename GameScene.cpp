#include "GameScene.h"
#include "Random.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete testSprite;
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


	//カメラ
	//カメラ初期化
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon->GetDevice());
	camera_->Initialize(input_);

	//描画初期化処理　ここから

	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon, WinApp::winW, WinApp::winH);


	//----------FBX----------

	//fbxLoadr汎用初期化
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera_);
	//グラフィックスパイプライン生成
	FbxObject3D::CreateGraphicsPipeline();

	//モデル名を指定してファイル読み込み
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	enemyModel = FbxLoader::GetInstance()->LoadModelFromFile("enemy");
	bReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("backReticle");
	fReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("frontReticle");

	//背景
	for (int i = 0; i < backGroundSize; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(backGroundNum,adjustPos);

		backGroundNum++;
		adjustPos += (backGroundNum * newBackGround->GetSize());

		backGrounds_.push_back(std::move(newBackGround));

	}

	//------------プレイヤー----------

	//プレイヤー初期化
	Player* newPlayer = new Player();
	newPlayer->Initialize(input_, playerModel, playerBulletModel, fReticleModel, bReticleModel);
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
		XMFLOAT3 md_pos = { 10.0f,5.0f,1.5f };
		XMFLOAT3 pos{};
		pos.x = Random(-md_pos.x, md_pos.x);
		pos.y = Random(-md_pos.y, md_pos.y);
		pos.z = Random(-md_pos.z, md_pos.z);

		pos.z = (oldPos + lowInterval);
		oldPos = pos.z;

		newObject->SetPosition(pos);
		enemys_.push_back(std::move(newObject));

	}

	//スプライトマネージャー
	SpriteManager::SetDevice(dxCommon->GetDevice());
	spriteManager = new SpriteManager;
	spriteManager->Initialize();

	//------テクスチャ------
	spriteManager->LoadFile(0, L"Resources/reimu.png");

	//-----スプライト------
	Sprite::SetDevice(dxCommon->GetDevice());
	Sprite::SetSpriteManager(spriteManager);
	Sprite::CreateGraphicsPipeLine();

	testSprite = new Sprite();
	testSprite->SetTextureNum(0);
	testSprite->Initialize();
	//アンカーポイントをスプライトの中心に
	testSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	testSprite->SetScale(XMFLOAT2(600, 300));
	testSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	testSprite->Update();
}

void GameScene::Update()
{
	//自機
	player_->Update();

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Update();
		}
	}

	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_)
	{
		backGround->Update();
	}

	//当たり判定
	Collition();

	//カメラ更新
	camera_->Update(player_->GetPosition());

	//パーティクルマネージャー静的更新
	ParticleManager::StaticUpdate(camera_->GetEye(), camera_->GetTarget());

}

void GameScene::Draw()
{

	//自機
	player_->Draw(dxCommon_->GetCommandList());

	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_) {
		backGround->Draw(dxCommon_->GetCommandList());
	}

	//スプライト
	testSprite->Draw(dxCommon_->GetCommandList());

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}

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

void GameScene::CheckEnemy()
{
	for (std::unique_ptr<Enemy>& enemy : enemys_) {

		//デスフラグ
		if (enemy->GetIsDead()) {
			enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->GetIsDead(); });
		}
	}
}
