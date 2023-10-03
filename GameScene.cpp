#include "GameScene.h"
#include "Random.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete titleSprite;
	delete clearSprite;
	//for (int i = 0; i < enemySize; i++)
	//{
	//	FBX_SAFE_DELETE(enemyModel);
	//}
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input_)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input_;

	//カメラ初期化
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon->GetDevice());
	camera_->Initialize(input_);

	//当たり判定
	collisionManager_ = new Collision();

	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon, WinApp::winW, WinApp::winH);

	//スプライトマネージャー
	SpriteManager::SetDevice(dxCommon->GetDevice());
	spriteManager = new SpriteManager;
	spriteManager->Initialize();

	//------テクスチャ------
	spriteManager->LoadFile(0, "title.png");
	spriteManager->LoadFile(1, "clear.png");

	//-----スプライト------
	Sprite::SetDevice(dxCommon->GetDevice());
	Sprite::SetSpriteManager(spriteManager);
	Sprite::CreateGraphicsPipeLine();

	titleSprite = new Sprite();
	titleSprite->SetTextureNum(0);
	titleSprite->Initialize();
	titleSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	titleSprite->SetScale(XMFLOAT2(1280, 720));
	titleSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

	clearSprite = new Sprite();
	clearSprite->SetTextureNum(1);
	clearSprite->Initialize();
	clearSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	clearSprite->SetScale(XMFLOAT2(1280, 720));
	clearSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

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
	phase = 1;


	//ボス
	Boss* newBoss = new Boss();
	newBoss->Initialize();
	boss_.reset(newBoss);
}

void GameScene::Update()
{

	//シーン更新
	switch (scene)
	{
	case TITLE:

		titleSprite->Update();

		break;

	case NORMALPLAY:

		//自機
		player_->Update();

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Update(player_->GetPosition(), player_->GetSpeed());
			}
		}

		//当たり判定
		Collition();

		break;
	case BOSSPLAY:

		//自機
		player_->Update();

		//ボス
		boss_->Update(player_->GetPosition(), player_->GetSpeed());

		//当たり判定
		BossSceneCollition();

		break;
	case CLEAR:

		clearSprite->Update();

		break;
	}

	//カメラ更新
	camera_->Update(player_->GetPosition(), boss_->GetPosition());

	//背景
	UpdateBackGround();

	//シーンの切り替わり
	ChangeScene();

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

	switch (scene) {

	case TITLE:

		break;

	case NORMALPLAY:

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Draw(dxCommon_->GetCommandList());
			}
		}

		//自機
		player_->Draw(dxCommon_->GetCommandList());

		break;

	case BOSSPLAY:

		//自機
		player_->Draw(dxCommon_->GetCommandList());

		//ボス
		boss_->Draw(dxCommon_->GetCommandList());

		break;

	case CLEAR:
		break;
	}
}

void GameScene::DrawSprite()
{
	switch (scene) {

	case TITLE:
		titleSprite->Draw(dxCommon_->GetCommandList());
		break;

	case NORMALPLAY:
		//レティクル
		player_->DrawRaticle(dxCommon_->GetCommandList());
		break;
	case BOSSPLAY:
		//レティクル
		player_->DrawRaticle(dxCommon_->GetCommandList());
		break;

	case CLEAR:
		clearSprite->Draw(dxCommon_->GetCommandList());
		break;

	}
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

		//フェーズが最大以下だったらまたフェーズ移行する
		if (phase < MaxPhase) {
			SetEnemy();
			phase++;
		}
		else {
			//フェーズが最終だったらシーン移行
			BossSceneInitialize();
		}
	}

}

void GameScene::UpdateBackGround()
{

	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_)
	{
		backGround->Update();

		//背景の位置をカメラが通り過ぎたら
		if (backGround->GetPosition().z + (backGround->GetSize() * 2) < camera_->GetEye().z) {

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
	int setNum = static_cast<int>(Random(0, enemyCSVSize - 0.001f));
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

void GameScene::Reset()
{
	//プレイヤー
	player_->Reset();

	//敵
	enemys_.clear();
	SetEnemy();
	phase = 1;

	//ボス
	boss_->Reset();

	//カメラ
	camera_->Update(player_->GetPosition(),boss_->GetPosition());

	//背景
	adjustPos = 0;
	for (std::unique_ptr<BackGround>& backGround : backGrounds_) {
		//オブジェクトを配置しなおす
		backGround->SetObject(adjustPos);
		//現在の位置+1つ分のサイズで次のマップの位置にセット
		adjustPos = backGround->GetPosition().z + backGround->GetSize();
	}

}

void GameScene::ChangeScene()
{
	//シーンの切り替え、そのタイミングでの情報の更新
	switch (scene)
	{
	case TITLE:

		if (input_->TriggerKey(DIK_SPACE)) {
			scene = NORMALPLAY;

			camera_->SetMode(PLAYERFOLLOWMODE);
		}

		break;

	case NORMALPLAY:

		//ボス戦ショートカット
		if (input_->TriggerKey(DIK_RETURN)) {
			BossSceneInitialize();


		}

		//敵のリストから削除要件確認
		CheckEnemy();

		break;
	case BOSSPLAY:

		//ボスが死んだら次のシーンへ
		if (boss_->GetIsDead()) {
			BossSceneFinalize();
		}

		break;
	case CLEAR:

		if (input_->TriggerKey(DIK_SPACE)) {
			scene = TITLE;
			Reset();

			camera_->SetMode(STRAIGHTMODE);
		}

		break;
	}
}

void GameScene::BossSceneInitialize()
{
	scene = BOSSPLAY;

	boss_->SetPosition(XMFLOAT3{ 0,0,player_->GetPosition().z + 26.0f });
}

void GameScene::BossSceneFinalize()
{
	scene = CLEAR;
}

void GameScene::BossSceneCollition()
{
#pragma region ボスと自機の弾の当たり判定

	if (player_->GetBulletSize() != 0) {

		for (int i = 0; i < player_->GetBulletSize(); i++) {


			if (!boss_->GetIsDead()) {
				//当たったか
				if (collisionManager_->CheckCollisionSquare(boss_->GetColData(), player_->GetBulletColData(i))) {

					//当たったらhpをへらしてパーティクル生成
					boss_->HitBullet();
					boss_->InitializeParticle();
					boss_->SetIsParticle(true);

					//自機の弾を消し、自機のスピードを上げスコアを加算
					player_->SetBulletIsDead(true, i);
					player_->SpeedUpByEnemy();
				}
			}

		}
	}

#pragma endregion 

#pragma region 自機とボスの弾


	if (boss_->GetBulletSize() != 0) {

		for (int i = 0; i < boss_->GetBulletSize(); i++) {

			if (!boss_->GetIsDead()) {
				//当たったか
				if (collisionManager_->CheckCollisionSquare(player_->GetColData(), boss_->GetBulletColData(i))) {

					//当たったら敵の弾を消し、自機のスピードを下げげスコアを減算
					boss_->SetBulletIsDead(true, i);
					player_->SpeedDownByEnemy();

				}
			}
		}
	}


#pragma endregion 
}
