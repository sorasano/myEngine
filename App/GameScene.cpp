/**
* @file GameScene.cpp
* @brief ゲームシーン
*/

#include "GameScene.h"
#include "MathManager.h"

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

void GameScene::Initialize()
{
	//インスタンスを取得
	this->dxCommon_ = DirectXCommon::GetInstance();;
	this->input_ = Input::GetInstance();

	//カメラ初期化
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon_->GetDevice());
	camera_->Initialize();

	//当たり判定
	collisionManager_ = new Collision();

	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon_);

	//スプライトマネージャー
	SpriteManager::SetDevice(dxCommon_->GetDevice());
	spriteManager_ = new SpriteManager;
	spriteManager_->Initialize();

	//------テクスチャ------
	spriteManager_->LoadFile(0, "title.png");
	spriteManager_->LoadFile(1, "clear.png");
	spriteManager_->LoadFile(2, "gameover.png");
	spriteManager_->LoadFile(3, "blue1x1.png");
	spriteManager_->LoadFile(4, "generalPurpose.png");

	//-----スプライト------
	Sprite::SetDevice(dxCommon_->GetDevice());
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

	gameoverSprite_ = new Sprite();
	gameoverSprite_->SetTextureNum(2);
	gameoverSprite_->Initialize();
	gameoverSprite_->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	gameoverSprite_->SetScale(XMFLOAT2(1280, 720));
	gameoverSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

	//----------FBX----------

	//fbxLoadr汎用初期化
	FbxLoader::GetInstance()->Initialize(dxCommon_->GetDevice());

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon_->GetDevice());
	FbxObject3D::SetCamera(camera_);
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
	Skydome* newSkydome = new Skydome();
	newSkydome->Initialize();
	skydome_.reset(newSkydome);

	//------------プレイヤー----------

	//プレイヤー初期化
	Player* newPlayer = new Player();
	newPlayer->Initialize();
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

	//演出
	performanceManager_ = new PerformanceManager();
	performanceManager_->Initialize(camera_, player_.get(),boss_.get());

	//パーティクルマネージャー
	particleManager_ = new ParticleManager();
	particleManager_->Initialize("Resources/effect/effect1.png");

}

void GameScene::Update()
{

	//シーン更新
	switch (scene_)
	{
	case TITLE:

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Update(player_->GetPosition(), player_->GetSpeed());
			}
		}

		//スプライト
		titleSprite_->Update();

		break;

	case PLAY:

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
	case BOSS:

		//自機
		player_->Update();

		//ボス
		boss_->Update(player_->GetPosition(), player_->GetSpeed());

		//当たり判定
		BossSceneCollition();

		break;
	case CLEAR:

		//自機
		player_->UpdateClearScene();

		//ボス
		boss_->UpdateClearScene();

		//スプライト
		clearSprite_->Update();

		break;

	case GAMEOVER:

		//スプライト
		gameoverSprite_->Update();

		break;
	}

	//カメラ更新
	camera_->Update(player_->GetPosition());

	//背景
	UpdateBackGround();

	//演出
	performanceManager_->Update();

	//パーティクル更新
	particleManager_->Update();

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

	case PLAY:

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

	case BOSS:

		//自機
		player_->Draw(dxCommon_->GetCommandList());

		//ボス
		boss_->Draw(dxCommon_->GetCommandList());

		break;

	case CLEAR:

		//自機
		player_->Draw(dxCommon_->GetCommandList());

		//ボス
		boss_->Draw(dxCommon_->GetCommandList());

		break;

	case GAMEOVER:

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

	case PLAY:
		player_->DrawSprite(dxCommon_->GetCommandList());
		break;
	case BOSS:
		if (!boss_->GetIsDead()) {
			player_->DrawSprite(dxCommon_->GetCommandList());
		}
		break;

	case CLEAR:
		clearSprite_->Draw(dxCommon_->GetCommandList());
		break;
	case GAMEOVER:
		gameoverSprite_->Draw(dxCommon_->GetCommandList());

	}

	//演出
	performanceManager_->DrawSprite(dxCommon_->GetCommandList());

	//パーティクル
	particleManager_->Draw();
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
						//敵撃破パーティクル生成
						particleManager_->MakeParticle(ENEMYDESTROY, enemy->GetPosition());

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
				//敵撃破パーティクル生成
				particleManager_->MakeParticle(ENEMYDESTROY, enemy->GetPosition());

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

	//デスフラグがtrueであればリストから削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->GetIsDead(); });

	//敵が0になったら
	if (enemys_.size() == 0) {

		//フェーズが最大以下だったらまたフェーズ移行する
		if (phase_ < MaxPhase_) {
			SetEnemy();
			phase_++;
		}
		else {
			//フェーズが最終だったらボスシーンへ
			if (!player_->GetAddSpeed()) {
				//加速スピードがない場合そのまま終了
				performanceManager_->SetPerformanceNum(GAMEOVERBOSS);
			}
			else {
				//加速スピードがある場合ボス戦へ
				performanceManager_->SetPerformanceNum(INBOSS);
			}

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

	//プレイヤー
	player_->Reset();

	//敵
	enemys_.clear();
	phase_ = 0;

	//ボス
	boss_->Reset();

	//カメラ
	camera_->SetMode(STRAIGHTMODE);
	//camera_->Update(player_->GetPosition(), boss_->GetPosition());

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


	//-----シーン切り替え時演出の開始-----
	switch (scene_)
	{
	case TITLE:

		//スペースを押したら演出プラスシーンの初期化
		if (input_->IsKeyTrigger(DIK_SPACE) || input_->IsPadTrigger(XINPUT_GAMEPAD_A) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(TITLETOPLAY);

			//プレイシーンは演出のため先に初期化をする
			PlaySceneInitialize();
		}

		break;

	case PLAY:

		//敵のリストから削除要件確認、フェーズもしくはシーン移行
		CheckEnemy();

		break;
	case BOSS:

		//ボスが死んだらクリア
		if (boss_->GetIsDead()) {
			performanceManager_->SetPerformanceNum(CLEARBOSS);
		}

		//プレイヤーのスピードが0になったらゲームオーバー
		if (!player_->GetAddSpeed()) {
			performanceManager_->SetPerformanceNum(GAMEOVERBOSS);
		}

		break;
	case CLEAR:

		//タイトルに戻る演出
		if (input_->IsKeyTrigger(DIK_SPACE) || input_->IsPadTrigger(XINPUT_GAMEPAD_A) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(RETURNTITLE);
		}

		break;

	case GAMEOVER:

		//汎用演出
		if (input_->IsKeyTrigger(DIK_SPACE) || input_->IsPadTrigger(XINPUT_GAMEPAD_A) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(RETURNTITLE);
		}

		break;
	}

	//-----演出終了でのシーン切り替え＋シーンの初期化-----
	if (scene_ != performanceManager_->GetIsChangeScene()) {
		scene_ = performanceManager_->GetIsChangeScene();
		switch (scene_)
		{
		case TITLE:

			Reset();
			break;

		case PLAY:

			break;
		case BOSS:

			BossSceneInitialize();
			break;
		case CLEAR:

			break;
		case GAMEOVER:

			break;
		}

	}
}

void GameScene::PlaySceneInitialize()
{

	//プレイヤー
	player_->SetPosition(XMFLOAT3{ camera_->GetEye().x,camera_->GetEye().y - 10,camera_->GetEye().z });
	player_->Update();

	//敵
	SetEnemy();
	phase_ = 1;

	//カメラ
	//camera_->Update(player_->GetPosition(), boss_->GetPosition());

}

void GameScene::BossSceneInitialize()
{
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

					//当たったらhpをへらす
					boss_->HitBullet();

					//自機の弾を消し、パーティクル生成
					player_->SetBulletIsDead(true, i);
					particleManager_->MakeParticle(PLAYERBULLETLANDING,player_->GetBulletPosition(i));

					//スコアを加算
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
