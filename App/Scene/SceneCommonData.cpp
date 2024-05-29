#include "SceneCommonData.h"

SceneCommonData::SceneCommonData()
{
	//インスタンスを取得
	this->dxCommon_ = MyEngine::DirectXCommon::GetInstance();;
	this->input_ = MyEngine::Input::GetInstance();

	//カメラ初期化
	Camera* newCamera = new Camera();
	newCamera->StaticInitialize(dxCommon_->GetDevice());
	newCamera->Initialize();
	camera_.reset(newCamera);

	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon_);

	//スプライトマネージャー
	SpriteManager::SetDevice(dxCommon_->GetDevice());
	SpriteManager* newSpriteManager = new SpriteManager();
	newSpriteManager->Initialize();
	spriteManager_.reset(newSpriteManager);

	//------テクスチャ------
	spriteManager_->LoadFile(0, "title.png");
	spriteManager_->LoadFile(1, "clear.png");
	spriteManager_->LoadFile(2, "gameover.png");
	spriteManager_->LoadFile(3, "blue1x1.png");
	spriteManager_->LoadFile(4, "generalPurpose.png");
	spriteManager_->LoadFile(5, "reticle.png");

	//メニュー用
	spriteManager_->LoadFile(6, "UI/menuUI.png");
	spriteManager_->LoadFile(7, "menu/menuBase.png");
	spriteManager_->LoadFile(8, "menu/menuSetting.png");
	spriteManager_->LoadFile(9, "menu/menuTitle.png");
	spriteManager_->LoadFile(10, "menu/menuClose.png");
	//メニュー設定画面
	spriteManager_->LoadFile(11, "menu/setting/settingBase.png");
	spriteManager_->LoadFile(12, "menu/setting/ON.png");
	spriteManager_->LoadFile(13, "menu/setting/OFF.png");
	spriteManager_->LoadFile(14, "menu/setting/settingMouse.png");
	//ガイド
	spriteManager_->LoadFile(15, "menu/guide.png");

	spriteManager_->LoadFile(16, "green1x1.png");

	spriteManager_->LoadFile(17, "UI/speedGauge.png");

	//-----スプライト------
	Sprite::SetDevice(dxCommon_->GetDevice());
	Sprite::SetSpriteManager(spriteManager_.get());
	Sprite::CreateGraphicsPipeLine();

	std::unique_ptr<Sprite> newTitleSprite = 
		std::make_unique<Sprite>(
			0, 
			XMFLOAT2(1280, 720),
			XMFLOAT2(window_width / 2, window_height / 2)
		);
	titleSprite_.swap(newTitleSprite);

	std::unique_ptr<Sprite> newClearSprite_ =
		std::make_unique<Sprite>(
			1,
			XMFLOAT2(1280, 720),
			XMFLOAT2(window_width / 2, window_height / 2)
		);
	clearSprite_.swap(newClearSprite_);

	std::unique_ptr<Sprite> newGameoverSprite_ = 
		std::make_unique<Sprite>(
			2,
			XMFLOAT2(1280, 720),
			XMFLOAT2(window_width / 2, window_height / 2)
		);
	gameoverSprite_.swap(newGameoverSprite_);

	std::unique_ptr<Sprite> newMenuUISprite_ = 
		std::make_unique<Sprite>(
			6,
			XMFLOAT2(100, 100),
			XMFLOAT2(window_width - 50, window_height - 50)
		);
	menuUISprite_.swap(newMenuUISprite_);

	//----------FBX----------

	//fbxLoadr汎用初期化
	FbxLoader::GetInstance()->Initialize(dxCommon_->GetDevice());

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon_->GetDevice());
	FbxObject3D::SetCamera(camera_.get());
	//グラフィックスパイプライン生成
	FbxObject3D::CreateGraphicsPipeline();

	//----------背景----------

	for (int i = 0; i < backGroundSize_; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(adjustPos_);

		//現在の位置+1つ分のサイズで次のマップの位置にセット
		adjustPos_ = newBackGround->GetPosition().z + newBackGround->GetSize();

		backGrounds_.push_back(std::move(newBackGround));
	}

	//スカイドーム初期化
	std::unique_ptr<Skydome> newSkydome = std::make_unique<Skydome>();
	newSkydome->Initialize();
	skydome_.swap(newSkydome);

	//------------プレイヤー----------

	//プレイヤー初期化
	std::unique_ptr<Player> newPlayer = std::make_unique<Player>();
	newPlayer->Initialize();
	player_.swap(newPlayer);

	//----------------敵--------------

	//モデル名を指定してファイル読み込み
	enemyModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("enemyred"));
	enemyBlueModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("enemyblue"));
	enemyYellowModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("enemyYellow"));

	enemyBulletModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("enemyBullet"));

	//csvファイル名前
	enemyCsvsName_ = {
	"enemy1",
	"enemy2",
	"enemy3",
	"enemy4",
	"enemy5"
	};

	enemyCSVSize_ = static_cast<int>(enemyCsvsName_.size());

	//csvデータをファイル数分,配列に入力
	for (int i = 0; i < enemyCSVSize_; i++) {

		//ファイルの名前を取得
		std::string fileName = enemyCsvsName_[i];

		std::unique_ptr<CSVLoader>newEnemyCsv = std::make_unique<CSVLoader>();
		newEnemyCsv->LoadCSV(fileName);
		enemyCsvs_.push_back(std::move(newEnemyCsv));

	}

	//ボス
	std::unique_ptr<Boss> newBoss = std::make_unique<Boss>();
	newBoss->Initialize();
	boss_.swap(newBoss);

	//演出
	std::unique_ptr<PerformanceManager> newPerformanceManager = std::make_unique<PerformanceManager>();
	newPerformanceManager->Initialize(camera_.get(), player_.get(), boss_.get());
	performanceManager_.swap(newPerformanceManager);

	//パーティクル

	//撃破
	std::unique_ptr<ParticleManager> newDestroyParticle_ = std::make_unique<ParticleManager>();
	newDestroyParticle_->Initialize("effect1.png");
	destroyParticle_.swap(newDestroyParticle_);

	//着弾
	std::unique_ptr<ParticleManager> newLandingParticle_ = std::make_unique<ParticleManager>();
	newLandingParticle_->Initialize("effect2.png");
	landingParticle_.swap(newLandingParticle_);

	//メニュー
	std::unique_ptr<Menu> newMenu = std::make_unique<Menu>();
	newMenu->Initialize();
	menu_.swap(newMenu);

}
