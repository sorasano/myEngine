#include "BossScene.h"

void BossScene::Initialize()
{
	cData_->boss_->SetPosition(XMFLOAT3{ 0,0,cData_->player_->GetPosition().z + 26.0f });
}

void BossScene::Update()
{
	//自機
	cData_->player_->Update(cData_->camera_->GetViewProjection());

	//ボス
	cData_->boss_->Update(cData_->player_->GetPosition(), cData_->player_->GetSpeed());


	//スプライト
	cData_->menuUISprite_->Update();

	//共通処理
	CommonUpdate();

	//シーン切り替え
	ChangeScene();
}

void BossScene::Draw()
{
	//共通処理
	CommonDraw();

	//自機
	cData_->player_->Draw(cData_->dxCommon_->GetCommandList());

	//ボス
	cData_->boss_->Draw(cData_->dxCommon_->GetCommandList());

}

void BossScene::DrawSprite()
{
	if (!cData_->performanceManager_->GetIsPerformance()) {
		cData_->player_->DrawSprite(cData_->dxCommon_->GetCommandList());
	}

	cData_->menuUISprite_->Draw(cData_->dxCommon_->GetCommandList());

	//共通処理
	CommonDrawSprite();
}

void BossScene::Finalize()
{
}

void BossScene::ChangeScene()
{

	//-----シーン切り替え時演出の開始-----
	//ボスが死んだらクリア
	if (cData_->boss_->GetIsDead()) {
		cData_->performanceManager_->SetPerformanceNum(CLEARBOSS);
	}

	//プレイヤーのスピードが0になったらゲームオーバー
	else if (!cData_->player_->GetAddSpeed()) {
		cData_->performanceManager_->SetPerformanceNum(GAMEOVERBOSS);
	}

	//メニュー
	if (collisionManager_->MenuUIColision()) {
		cData_->performanceManager_->SetIsOldScene(BOSS);
		cData_->performanceManager_->SetPerformanceNum(OPENMENU);

	}

	//-----演出終了でのシーン切り替え-----
	if (cData_->scene_ != cData_->performanceManager_->GetIsChangeScene()) {

		//前シーンがメニューシーンか
		bool oldSceneMenu = false;
		if (cData_->scene_ == MENU) {
			oldSceneMenu = true;
		}

		//シーンを切り替え
		cData_->scene_ = cData_->performanceManager_->GetIsChangeScene();

		if (cData_->scene_ == CLEAR) {
			//次シーンの生成
			BaseScene* scene = new ClearScene(cData_, collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}else if (cData_->scene_ == GAMEOVER) {
			//次シーンの生成
			BaseScene* scene = new GameoverScene(cData_, collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}else if (cData_->scene_ == MENU) {
			//次シーンの生成
			BaseScene* scene = new MenuScene(cData_, collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
	}
}

