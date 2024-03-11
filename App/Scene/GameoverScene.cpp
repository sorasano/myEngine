#include "GameoverScene.h"

void GameoverScene::Initialize()
{
}

void GameoverScene::Update()
{

	//自機
	cData_->player_->UpdateGameoverScene();

	//ボス
	cData_->boss_->UpdateGameoverScene();

	//スプライト
	cData_->gameoverSprite_->Update();

	//共通処理
	CommonUpdate();

	//シーン切り替え
	ChangeScene();
}

void GameoverScene::Draw()
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

	//ボス
	cData_->boss_->Draw(cData_->dxCommon_->GetCommandList());

}

void GameoverScene::DrawSprite()
{
	cData_->gameoverSprite_->Draw(cData_->dxCommon_->GetCommandList());

	//共通処理
	CommonDrawSprite();

}

void GameoverScene::Finalize()
{
}

void GameoverScene::ChangeScene()
{
	//汎用演出
	if (cData_->input_->IsMouseTrigger(LEFT_CLICK) && !cData_->performanceManager_->GetIsPerformance()) {
		cData_->performanceManager_->SetPerformanceNum(RETURNTITLE);

		//次シーンの生成
		BaseScene* scene = new TitleScene(cData_);
		//シーン切り替え依頼
		sceneManager_->SetNextScene(scene);
	}

	//-----演出終了でのシーン切り替え-----
	if (cData_->scene_ != cData_->performanceManager_->GetIsChangeScene()) {

		//シーンを切り替え
		cData_->scene_ = cData_->performanceManager_->GetIsChangeScene();

		if (cData_->scene_ == TITLE) {
			//次シーンの生成
			BaseScene* scene = new TitleScene(cData_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
	}
}

void GameoverScene::Collition()
{
}
