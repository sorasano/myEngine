/**
* @file GameScene.cpp
* @brief ゲームシーン
*/

#include "GameScene.h"
#include "Random.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete titleSprite_;
	delete clearSprite_;
	//for (int i = 0; i < enemySize; i++)
	//{
	//	FBX_SAFE_DELETE(enemyModel);
	//}
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input;

	//カメラ初期化
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon->GetDevice());
	camera_->Initialize(input_);

	//当たり判定
	collisionManager_ = new Collision();

	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon);

	//スプライトマネージャー
	SpriteManager::SetDevice(dxCommon->GetDevice());
	spriteManager_ = new SpriteManager;
	spriteManager_->Initialize();

	//------テクスチャ------
	spriteManager_->LoadFile(0, "title.png");
	spriteManager_->LoadFile(1, "clear.png");
	spriteManager_->LoadFile(2, "blue1x1.png");
	spriteManager_->LoadFile(3, "generalPurpose.png");

	//-----スプライト------
	Sprite::SetDevice(dxCommon->GetDevice());
	Sprite::SetSpriteManager(spriteManager_);
	Sprite::CreateGraphicsPipeLine();

	titleSprite_ = new Sprite();
	titleSprite_->SetTextureNum(0);
	titleSprite_->Initialize();
	titleSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	titleSprite_->SetScale(XMFLOAT2(1280, 720));
	titleSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

	clearSprite_ = new Sprite();
	clearSprite_->SetTextureNum(1);
	clearSprite_->Initialize();
	clearSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	clearSprite_->SetScale(XMFLOAT2(1280, 720));
	clearSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

	//----------FBX----------

	//fbxLoadr汎用初期化
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera_);
	//グラフィックスパイプライン生成
	FbxObject3D::CreateGraphicsPipeline();

	//演出
	performanceManager_ = new PerformanceManager();
	performanceManager_->Initialize(camera_);

	//----------背景----------

	for (int i = 0; i < backGroundSize_; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(adjustPos_);

		//現在の位置+1つ分のサイズで次のマップの位置にセット
		adjustPos_ = newBackGround->GetPosition().z + newBackGround->GetSize();

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
	enemyModel_ = FbxLoader::GetInstance()->LoadModelFromFile("enemy");
	enemyBulletModel_ = FbxLoader::GetInstance()->LoadModelFromFile("enemyBullet");

	//csvファイル名前
	enemyCsvsName_ = {
	"enemy1",
	"enemy2",
	"enemy3"
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
	Boss* newBoss = new Boss();
	newBoss->Initialize();
	boss_.reset(newBoss);

}

void GameScene::Update()
{

	//シーン更新
	switch (scene_)
	{
	case TITLE:

		titleSprite_->Update();

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Update(player_->GetPosition(), player_->GetSpeed());
			}
		}

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

		clearSprite_->Update();

		break;
	}

	//カメラ更新
	camera_->Update(player_->GetPosition(), boss_->GetPosition());

	//背景
	UpdateBackGround();

	//演出
	performanceManager_->Update();

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

	switch (scene_) {

	case TITLE:

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Draw(dxCommon_->GetCommandList());
			}
		}

		//自機
		//player_->Draw(dxCommon_->GetCommandList());

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
	switch (scene_) {

	case TITLE:
		if (!performanceManager_->GetIsPerformance()) {
			titleSprite_->Draw(dxCommon_->GetCommandList());
		}
		break;

	case NORMALPLAY:
		player_->DrawSprite(dxCommon_->GetCommandList());
		break;
	case BOSSPLAY:
		player_->DrawSprite(dxCommon_->GetCommandList());
		break;

	case CLEAR:
		clearSprite_->Draw(dxCommon_->GetCommandList());
		break;
	}

	//演出
	performanceManager_->DrawSprite(dxCommon_->GetCommandList());
}

bool GameScene::UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	//スカイドームまでを更新描画
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -skydome_->GetEdge()) { return true; }

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
					if (collisionManager_->CheckSquareToSquare(enemy->GetColData(), player_->GetBulletColData(i))) {

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
			if (collisionManager_->CheckSquareToSquare(enemy->GetColData(), player_->GetColData())) {

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
				if (collisionManager_->CheckSquareToSquare(it1->get()->GetColData(), it2->get()->GetColData())) {

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
					if (collisionManager_->CheckSquareToSquare(player_->GetColData(), enemy->GetBulletColData(i))) {

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
		if (phase_ < MaxPhase_) {
			SetEnemy();
			phase_++;
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
		if (backGround->GetPosition().z + backGround->GetSize() * 2.5 < camera_->GetEye().z) {

			//過ぎたオブジェクトを削除
			backGround->DeleteObject();

			//オブジェクトを配置しなおす
			backGround->SetObject(adjustPos_);

			//現在の位置+1つ分のサイズで次のマップの位置にセット
			adjustPos_ = backGround->GetPosition().z + backGround->GetSize();

		}

	}

	//スカイドーム
	skydome_->Update(camera_->GetEye().z);

}

void GameScene::SetEnemy()
{

	//発生させる位置はスカイドームの端
	float makePos = player_->GetPosition().z + skydome_->GetEdge();

	//何番目のCSVをセットするか(ランダム)
	int setNum = static_cast<int>(Random(0, enemyCSVSize_ - 0.001f));
	auto it = enemyCsvs_.begin();
	setNum = 2;
	std::advance(it, setNum);

	for (int i = 0; i < it->get()->GetSize(); i++)
	{
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(enemyModel_, enemyBulletModel_);

		newObject->SetPosition(XMFLOAT3(it->get()->GetPosition(i).x, it->get()->GetPosition(i).y, it->get()->GetPosition(i).z + makePos));
		newObject->SetType(it->get()->GetType(i));
		newObject->SetStopInScreen(it->get()->GetStopInScreen(i));

		enemys_.push_back(std::move(newObject));
	}
}

void GameScene::Reset()
{
	//リセットはタイトルシーンに戻る
	scene_ = TITLE;
	camera_->SetMode(STRAIGHTMODE);

	//プレイヤー
	player_->Reset();

	//敵
	enemys_.clear();

	//ボス
	boss_->Reset();

	//カメラ
	camera_->Update(player_->GetPosition(), boss_->GetPosition());

	//背景
	//オブジェクトを全削除
	//backGrounds_.clear();

	////オブジェクトを配置しなおす
	//adjustPos_ = 0;
	//for (int i = 0; i < backGroundSize_; i++) {
	//	std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
	//	newBackGround->Initialize(adjustPos_);
	//	//現在の位置+1つ分のサイズで次のマップの位置にセット
	//	adjustPos_ = newBackGround->GetPosition().z + newBackGround->GetSize();
	//	backGrounds_.push_back(std::move(newBackGround));
	//}

}

void GameScene::ChangeScene()
{
	//シーンの切り替え、そのタイミングでの情報の更新
	switch (scene_)
	{
	case TITLE:

		////スペースを押したら演出プラスシーンの初期化
		//if (input_->TriggerKey(DIK_SPACE) && !performanceManager_->GetIsPerformance()) {
		//	performanceManager_->SetPerformanceNum(TITLETOPLAY);
		//	PlaySceneInitialize();
		//}
		////シーン切り替えフラグがたったらシーン切り替え
		//if (performanceManager_->GetIsChangeScene()) {
		//	scene_ = NORMALPLAY;
		//	performanceManager_->SetIsChangeScene(false);
		//}

		//汎用演出ショートカット
		if (input_->TriggerKey(DIK_RETURN) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(GENERALPURPOSE);
		}
		if (performanceManager_->GetIsChangeScene()) {
			Reset();
			performanceManager_->SetIsChangeScene(false);
		}

		break;

	case NORMALPLAY:

		//ボス戦ショートカット
		//if (input_->TriggerKey(DIK_RETURN)) {
		//	BossSceneInitialize();
		//}

		//敵のリストから削除要件確認
		CheckEnemy();

		break;
	case BOSSPLAY:

		//ボスが死んだら次のシーンへ
		if (boss_->GetIsDead()) {
			scene_ = CLEAR;
		}

		break;
	case CLEAR:


		//汎用演出
		if (input_->TriggerKey(DIK_SPACE) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(GENERALPURPOSE);
		}
		else if (performanceManager_->GetIsChangeScene()) {
			Reset();
			performanceManager_->SetIsChangeScene(false);
		}

		break;
	}

}

void GameScene::PlaySceneInitialize()
{

	//プレイヤー
	player_->SetPosition(XMFLOAT3{ camera_->GetEye().x,camera_->GetEye().y  - 10,camera_->GetEye().z });
	player_->Update();

	//敵
	SetEnemy();
	phase_ = 1;

	//カメラ
	camera_->Update(player_->GetPosition(), boss_->GetPosition());
	camera_->SetMode(TITLETOPLAYMODE);

}

void GameScene::BossSceneInitialize()
{
	scene_ = BOSSPLAY;

	boss_->SetPosition(XMFLOAT3{ 0,0,player_->GetPosition().z + 26.0f });
}

void GameScene::BossSceneCollition()
{
#pragma region ボスと自機の弾の当たり判定

	if (player_->GetBulletSize() != 0) {

		for (int i = 0; i < player_->GetBulletSize(); i++) {


			if (!boss_->GetIsDead()) {
				//当たったか
				if (collisionManager_->CheckSquareToSquare(boss_->GetColData(), player_->GetBulletColData(i))) {

					//当たったらhpをへらしてパーティクル生成
					boss_->HitBullet();
					boss_->InitializeParticle();
					boss_->SetIsParticle(true);

					//自機の弾を消し、スコアを加算
					player_->SetBulletIsDead(true, i);

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
				if (collisionManager_->CheckSquareToSquare(player_->GetColData(), boss_->GetBulletColData(i))) {

					//当たったら敵の弾を消し、自機のスピードを下げスコアを減算
					boss_->SetBulletIsDead(true, i);
					player_->SpeedDownByEnemy();

				}
			}
		}
	}


#pragma endregion 
}
