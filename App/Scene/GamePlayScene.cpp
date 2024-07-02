#include "GamePlayScene.h"

void GamePlayScene::Initialize()
{

}

void GamePlayScene::Update()
{
	//自機
	cData_->player_->Update(cData_->camera_->GetViewProjection());

	//敵
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (UpadateRange(cData_->camera_->GetEye(), enemy->GetPosition())) {
			enemy->Update(
				cData_->player_->GetPosition(), 
				cData_->player_->GetSpeed(), 
				cData_->camera_->GetViewProjection(),
				cData_->camera_->GetEye().z
			);
		}
	}

	//スプライト
	cData_->menuUISprite_->Update();

	//共通処理
	CommonUpdate();

	//シーン切り替え
	ChangeScene();
}

void GamePlayScene::Draw()
{
	//共通処理
	CommonDraw();

	//敵
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (UpadateRange(cData_->camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(cData_->dxCommon_->GetCommandList());
		}
	}

	//自機
	cData_->player_->Draw(cData_->dxCommon_->GetCommandList());
}

void GamePlayScene::DrawSprite()
{
	if (!cData_->performanceManager_->GetIsPerformance()) {
		cData_->player_->DrawSprite(cData_->dxCommon_->GetCommandList());
	}

	cData_->menuUISprite_->Draw(cData_->dxCommon_->GetCommandList());

	//共通処理
	CommonDrawSprite();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::ChangeScene()
{
	//敵のリストから削除要件確認、フェーズもしくはシーン移行
	CheckEnemy();

	//メニュー
	if (collisionManager_->MenuUIColision()) {
		cData_->performanceManager_->SetIsOldScene(PLAY);
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

		if (cData_->scene_ == BOSS) {
			//次シーンの生成
			BaseScene* scene = new BossScene(collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
		else if (cData_->scene_ == GAMEOVER) {
			//次シーンの生成
			BaseScene* scene = new GameoverScene(collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
		else if (cData_->scene_ == MENU) {
			//次シーンの生成
			BaseScene* scene = new MenuScene(collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
	}
}

void GamePlayScene::CheckEnemy()
{
	//敵削除処理
	DeleteEnemy();

	//敵が0になったら
	if (cData_->enemys_.size() == 0) {

		//フェーズが最大以下だったらまたフェーズ移行する
		if (cData_->phase_ < cData_->MaxPhase_) {
			SetEnemy();
			cData_->phase_++;
		}
		else {
			//フェーズが最終だったらボスシーンへ
			if (!cData_->player_->GetAddSpeed()) {
				//加速スピードがない場合そのまま終了
				cData_->performanceManager_->SetPerformanceNum(GAMEOVERBOSS);
			}
			else {
				//加速スピードがある場合ボス戦へ
				cData_->performanceManager_->SetPerformanceNum(INBOSS);
			}

		}
	}
}

void GamePlayScene::DeleteEnemy()
{

	for (int i = 0; i < cData_->enemys_.size(); i++) {

		//カメラより後ろに行ったら死亡
		if (cData_->enemys_[i]->GetPosition().z < cData_->camera_->GetEye().z) {
			cData_->enemys_[i]->SetISDesd(true);
		}

		//デスフラグがtrueであればリストから削除
		if (cData_->enemys_[i]->GetIsDead()) {
			cData_->enemys_.erase(cData_->enemys_.begin() + i);
		}
	}

}
