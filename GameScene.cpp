#include "GameScene.h"
#include "Random.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete testSprite;
	//for (int i = 0; i < enemySize; i++)
	//{
	//	FBX_SAFE_DELETE(enemyModel);
	//}
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

	//当たり判定
	collisionManager_ = new Collision();

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

	//----------背景----------

	for (int i = 0; i < backGroundSize; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(adjustPos);

		//現在の位置+1つ分のサイズで次のマップの位置にセット
		adjustPos = newBackGround->GetPosition().z + newBackGround->GetSize();

		backGrounds_.push_back(std::move(newBackGround));
	}

	//スカイドーム初期化
	Skydome* newSkydome = new Skydome();
	newSkydome->Initialize();
	skydome_.reset(newSkydome);

	//------------プレイヤー----------

	//プレイヤー初期化
	Player* newPlayer = new Player();
	newPlayer->Initialize(input_);
	player_.reset(newPlayer);

	//----------------敵--------------

	//モデル名を指定してファイル読み込み
	enemyModel = FbxLoader::GetInstance()->LoadModelFromFile("enemy");
	enemyBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("enemyBullet");

	//csvファイル名前
	enemyCsvsName_ = {
	"enemy1",
	"enemy2",
	"enemy3"
	};

	enemyCSVSize = static_cast<int>(enemyCsvsName_.size());

	//csvデータをファイル数分,配列に入力
	for (int i = 0; i < enemyCSVSize; i++) {

		//ファイルの名前を取得
		std::string fileName = enemyCsvsName_[i];

		std::unique_ptr<CSVLoader>newEnemyCsv = std::make_unique<CSVLoader>();
		newEnemyCsv->LoadCSV(fileName);
		enemyCsvs_.push_back(std::move(newEnemyCsv));

	}

	//csvデータをもとにに敵にデータをセット
	SetEnemy();
	
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
			enemy->Update(player_->GetPosition(), player_->GetSpeed());
		}
	}

	//敵のリストから削除要件確認
	CheckEnemy();

	//背景
	UpdateBackGround();

	//当たり判定
	Collition();

	//カメラ更新
	camera_->Update(player_->GetPosition());

	//パーティクルマネージャー静的更新
	ParticleManager::StaticUpdate(camera_->GetEye(), camera_->GetTarget());

}

void GameScene::Draw()
{

	//スカイドーム
	skydome_->Draw(dxCommon_->GetCommandList());

	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_) {
		if (UpadateRange(camera_->GetEye(), backGround->GetPosition())) {
			backGround->Draw(dxCommon_->GetCommandList());
		}
	}

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}

	//自機
	player_->Draw(dxCommon_->GetCommandList());

}

void GameScene::DrawSprite()
{
	//スプライト
	testSprite->Draw(dxCommon_->GetCommandList());

	//レティクル
	player_->DrawRaticle(dxCommon_->GetCommandList());
}

bool GameScene::UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -rangeMaxZ) { return true; }

	return false;
}

void GameScene::Collition()
{
#pragma region 敵と自機の弾の当たり判定

	if (player_->GetBulletSize() != 0) {

		for (int i = 0; i < player_->GetBulletSize(); i++) {

			for (std::unique_ptr<Enemy>& enemy : enemys_)
			{
				if (!enemy->GetIsDead()) {
					//当たったか
					if (collisionManager_->CheckCollisionSquare(enemy->GetColData(), player_->GetBulletColData(i))) {

						//当たったら敵は消してパーティクル生成
						enemy->SetISDesd(true);
						enemy->InitializeParticle();

						//自機の弾を消し、自機のスピードを上げスコアを加算
						player_->SetBulletIsDead(true, i);
						player_->SpeedUpByEnemy();
					}
				}
			}
		}
	}

#pragma endregion 

#pragma region	敵と自機の当たり判定

	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (!enemy->GetIsDead()) {
			//当たったか
			if (collisionManager_->CheckCollisionSquare(enemy->GetColData(), player_->GetColData())) {

				//当たったら敵は消してパーティクル生成
				enemy->SetISDesd(true);
				enemy->InitializeParticle();

				//自機のスピードを下げ,少し無敵時間に
				player_->SpeedDownByEnemy();
				player_->SetIsInvincible(true);

			}
		}
	}

#pragma endregion 

#pragma region	敵と敵

	const int enemySize = static_cast<const int>(enemys_.size());

	for (int i = 0; i < enemySize; i++) {
		for (int j = 0; j < enemySize; j++) {

			if (i < j) {
				break;
			}

			auto it1 = enemys_.begin();
			std::advance(it1, i);

			auto it2 = enemys_.begin();
			std::advance(it2, j);

			if (!it1->get()->GetIsDead() && !it2->get()->GetIsDead()) {

				//当たったか
				if (collisionManager_->CheckCollisionSquare(it1->get()->GetColData(), it2->get()->GetColData())) {

					//当たったら反射させる
					it1->get()->Reflection();
					it2->get()->Reflection();

				}

			}

		}
	}

#pragma endregion 

#pragma region 自機と敵の弾

	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (enemy->GetBulletSize() != 0) {

			for (int i = 0; i < enemy->GetBulletSize(); i++) {

				if (!enemy->GetIsDead()) {
					//当たったか
					if (collisionManager_->CheckCollisionSquare(player_->GetColData(), enemy->GetBulletColData(i))) {

						//当たったら敵の弾を消し、自機のスピードを下げげスコアを減算
						enemy->SetBulletIsDead(true, i);
						player_->SpeedDownByEnemy();

					}
				}
			}
		}
	}

#pragma endregion 

}


void GameScene::CheckEnemy()
{
	for (std::unique_ptr<Enemy>& enemy : enemys_) {

		//カメラより後ろに行ったら死亡
		if (enemy->GetPosition().z < camera_->GetEye().z) {
			enemy->SetISDesd(true);
		}

	}

	//デスフラグがtrueでパーティクル演出もなければリストから削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->GetIsDead() && !enemy->GetIsParticle(); });

	//敵が0になったら
	if (enemys_.size() == 0) {

		SetEnemy();

	}

}

void GameScene::UpdateBackGround()
{

	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_)
	{
		backGround->Update();

		//背景の位置をカメラが通り過ぎたら
		if (backGround->GetPosition().z + backGround->GetSize() < camera_->GetEye().z) {

			//過ぎたオブジェクトを削除
			backGround->DeleteObject();

			//オブジェクトを配置しなおす
			backGround->SetObject(adjustPos);

			//現在の位置+1つ分のサイズで次のマップの位置にセット
			adjustPos = backGround->GetPosition().z + backGround->GetSize();

		}

	}

	//スカイドーム
	skydome_->Update(camera_->GetEye().z);

}

void GameScene::SetEnemy()
{

	//発生させる位置は最大描画距離から
	float makePos = player_->GetPosition().z + rangeMaxZ;

	//何番目のCSVをセットするか(ランダム)
	int setNum = static_cast<int>(Random(0,enemyCSVSize - 0.001f));
	auto it = enemyCsvs_.begin();
	std::advance(it, setNum);

	for (int i = 0; i < it->get()->GetSize(); i++)
	{
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(enemyModel, enemyBulletModel);

		newObject->SetPosition(XMFLOAT3(it->get()->GetPosition(i).x, it->get()->GetPosition(i).y, it->get()->GetPosition(i).z + makePos));
		newObject->SetType(it->get()->GetType(i));
		newObject->SetStopInScreen(it->get()->GetStopInScreen(i));

		enemys_.push_back(std::move(newObject));
	}
}
