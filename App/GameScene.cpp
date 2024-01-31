/**
* @file GameScene.cpp
* @brief ゲームシーン
*/

#include "GameScene.h"
#include "Random.h"
#include "imgui.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Initialize()
{
	//インスタンスを取得
	this->dxCommon_ = DirectXCommon::GetInstance();;
	this->input_ = Input::GetInstance();
	this->collisionManager_ = Collision::GetInstance();


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


	//-----スプライト------
	Sprite::SetDevice(dxCommon_->GetDevice());
	Sprite::SetSpriteManager(spriteManager_.get());
	Sprite::CreateGraphicsPipeLine();

	Sprite* newTitleSprite = new Sprite();
	newTitleSprite->SetTextureNum(0);
	newTitleSprite->Initialize();
	newTitleSprite->SetScale(XMFLOAT2(1280, 720));
	newTitleSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	titleSprite_.reset(newTitleSprite);

	Sprite* newClearSprite_ = new Sprite();
	newClearSprite_->SetTextureNum(1);
	newClearSprite_->Initialize();
	newClearSprite_->SetScale(XMFLOAT2(1280, 720));
	newClearSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	clearSprite_.reset(newClearSprite_);

	Sprite* newGameoverSprite_ = new Sprite();
	newGameoverSprite_->SetTextureNum(2);
	newGameoverSprite_->Initialize();
	newGameoverSprite_->SetScale(XMFLOAT2(1280, 720));
	newGameoverSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	gameoverSprite_.reset(newGameoverSprite_);

	Sprite* newMenuUISprite_ = new Sprite();
	newMenuUISprite_->SetTextureNum(6);
	newMenuUISprite_->Initialize();
	newMenuUISprite_->SetScale(XMFLOAT2(100, 100));
	newMenuUISprite_->SetPosition(XMFLOAT2(window_width - 50, window_height - 50));
	menuUISprite_.reset(newMenuUISprite_);

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
	Boss* newBoss = new Boss();
	newBoss->Initialize();
	boss_.reset(newBoss);

	//演出
	PerformanceManager* newPerformanceManager = new PerformanceManager();
	newPerformanceManager->Initialize(camera_.get(), player_.get(), boss_.get());
	performanceManager_.reset(newPerformanceManager);

	//パーティクルマネージャー
	ParticleManager* newParticleManager = new ParticleManager();
	newParticleManager->Initialize("Resources/effect/effect1.png");
	particleManager_.reset(newParticleManager);

	//メニュー
	Menu* newMenu = new Menu();
	newMenu->Initialize();
	menu_.reset(newMenu);

}

void GameScene::Update()
{

	if (input_->IsKeyPress(DIK_1)) {
		input_->WindowLock();
	}
	else if (input_->IsKeyPress(DIK_2)) {
		input_->WindowUnLock();
	}

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
		player_->Update(camera_->GetViewProjection());

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Update(player_->GetPosition(), player_->GetSpeed());
			}
		}

		//当たり判定
		Collition();

		//スプライト
		menuUISprite_->Update();
		break;
	case BOSS:

		//自機
		player_->Update(camera_->GetViewProjection());

		//ボス
		boss_->Update(player_->GetPosition(), player_->GetSpeed());

		//当たり判定
		BossSceneCollition();

		//スプライト
		menuUISprite_->Update();
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

		//自機
		player_->UpdateGameoverScene();

		//ボス
		boss_->UpdateGameoverScene();

		//スプライト
		gameoverSprite_->Update();
		break;

	case MENU:

		menu_->Update();

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

	//ImGui::Begin("cameraposZ");
	//ImGui::Text("%f,", camera_->GetEye().z);
	//ImGui::End();

	//ImGui::Begin("playerposZ");
	//ImGui::Text("%f,", player_->GetPosition().z);
	//ImGui::End();
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
		player_->DrawClearScene(dxCommon_->GetCommandList());

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

		//ボス
		boss_->Draw(dxCommon_->GetCommandList());

		break;

	case MENU:

		//自機
		player_->Draw(dxCommon_->GetCommandList());

		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys_)
		{
			if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
				enemy->Draw(dxCommon_->GetCommandList());
			}
		}

		//ボス
		boss_->Draw(dxCommon_->GetCommandList());

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
		if (!performanceManager_->GetIsPerformance()) {
			player_->DrawSprite(dxCommon_->GetCommandList());
		}

		menuUISprite_->Draw(dxCommon_->GetCommandList());

		break;
	case BOSS:

		if (!performanceManager_->GetIsPerformance()) {
			player_->DrawSprite(dxCommon_->GetCommandList());
		}

		menuUISprite_->Draw(dxCommon_->GetCommandList());

		break;

	case CLEAR:
		clearSprite_->Draw(dxCommon_->GetCommandList());
		break;
	case GAMEOVER:
		gameoverSprite_->Draw(dxCommon_->GetCommandList());
		break;

	case MENU:
		menu_->Draw(dxCommon_->GetCommandList());

		break;
	}

	//演出

	performanceManager_->DrawSprite(dxCommon_->GetCommandList());

	//パーティクル
	particleManager_->Draw();
}

bool GameScene::UpadateRange(const XMFLOAT3& cameraPos, const XMFLOAT3& pos)
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
	std::advance(it, setNum);

	for (int i = 0; i < it->get()->GetSize(); i++)
	{
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(enemyModel_.get(), enemyBulletModel_.get());

		if (it->get()->GetType(i) == 1 || it->get()->GetType(i) == 2) {
			newObject->SetModel(enemyBlueModel_.get());
		}
		else if (it->get()->GetType(i) > 2) {
			newObject->SetModel(enemyYellowModel_.get());
		}

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
	camera_->Reset();

	menu_->Reset();


	//背景
	//オブジェクトを全削除
	backGrounds_.clear();

	//オブジェクトを配置しなおす
	adjustPos_ = 0;
	for (int i = 0; i < backGroundSize_; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(adjustPos_);
		//現在の位置+1つ分のサイズで次のマップの位置にセット
		adjustPos_ = newBackGround->GetPosition().z + newBackGround->GetSize();
		backGrounds_.push_back(std::move(newBackGround));
	}

}

void GameScene::ChangeScene()
{
	//シーンの切り替え、そのタイミングでの情報の更新


	//-----シーン切り替え時演出の開始-----
	switch (scene_)
	{
	case TITLE:

		//左クリックしたら演出プラスシーンの初期化
		if (input_->IsMouseTrigger(LEFT_CLICK) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(TITLETOPLAY);

			//プレイシーンは演出のため先に初期化をする
			PlaySceneInitialize();
		}

		break;

	case PLAY:

		//敵のリストから削除要件確認、フェーズもしくはシーン移行
		CheckEnemy();

		//メニュー
		if (MenuUIColision()) {
			performanceManager_->SetIsOldScene(PLAY);
			performanceManager_->SetPerformanceNum(OPENMENU);
		}

		break;
	case BOSS:

		//ボスが死んだらクリア
		if (boss_->GetIsDead()) {
			performanceManager_->SetPerformanceNum(CLEARBOSS);
		}

		//プレイヤーのスピードが0になったらゲームオーバー
		else if (!player_->GetAddSpeed()) {
			performanceManager_->SetPerformanceNum(GAMEOVERBOSS);
		}

		//メニュー
		if (MenuUIColision()) {
			performanceManager_->SetIsOldScene(BOSS);
			performanceManager_->SetPerformanceNum(OPENMENU);
		}

		break;
	case CLEAR:

		//タイトルに戻る演出
		if (input_->IsMouseTrigger(LEFT_CLICK) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(RETURNTITLE);
		}

		break;

	case GAMEOVER:

		//汎用演出
		if (input_->IsMouseTrigger(LEFT_CLICK) && !performanceManager_->GetIsPerformance()) {
			performanceManager_->SetPerformanceNum(RETURNTITLE);
		}

		break;

	case MENU:

		//メニューでシーン切り替えフラグがたったら
		if (menu_->GetIsSerect() && !performanceManager_->GetIsPerformance()) {

			//選択中の物を参照してシーン遷移
			if (menu_->GetSerect() == MENUTITLE) {
				//タイトルに戻る演出
				performanceManager_->SetPerformanceNum(RETURNTITLE);
			}
			else if (menu_->GetSerect() == MENUCLOSE) {
				//メニューを閉じる
				performanceManager_->SetPerformanceNum(CLOSEMENU);
			}

			//メニューを閉じたとき用リセット
			menu_->CloseReset();

		}

		break;
	}

	//-----演出終了でのシーン切り替え＋シーンの初期化-----
	if (scene_ != performanceManager_->GetIsChangeScene()) {

		//前シーンがメニューシーンか
		bool oldSceneMenu = false;
		if (scene_ == MENU) {
			oldSceneMenu = true;
		}

		//シーンを切り替え
		scene_ = performanceManager_->GetIsChangeScene();

		switch (scene_)
		{
		case TITLE:

			Reset();
			break;

		case PLAY:

			break;
		case BOSS:

			//メニューシーンからの切り替えは初期化しない
			if (!oldSceneMenu) {
				BossSceneInitialize();
			}

			break;
		case CLEAR:

			break;
		case GAMEOVER:

			break;

		case MENU:

			break;
		}

	}

	//ImGui::Begin("scene");
	//ImGui::Text("%d,", scene);
	//ImGui::End();


}

bool GameScene::MenuUIColision()
{
	XMFLOAT2 mousePos = input_->GetMousePosition();

	if (collisionManager_->CheckSpriteTo2Dpos(menuUISprite_.get(), mousePos)) {

		performanceManager_->MenuUIRotPerformance(menuUISprite_.get());

		if (input_->IsMouseTrigger(LEFT_CLICK)) {

			return true;
		}
	}

	return false;

}

void GameScene::PlaySceneInitialize()
{

	//プレイヤー
	player_->SetPosition(XMFLOAT3{ camera_->GetEye().x,camera_->GetEye().y - 10,camera_->GetEye().z });
	//player_->Update();

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
					particleManager_->MakeParticle(PLAYERBULLETLANDING, player_->GetBulletPosition(i));

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
