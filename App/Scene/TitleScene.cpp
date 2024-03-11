#include "TitleScene.h"

void TitleScene::Initialize()
{
	Reset();
}

void TitleScene::Update()
{
	//敵
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (UpadateRange(cData_->camera_->GetEye(), enemy->GetPosition())) {
			enemy->Update(cData_->player_->GetPosition(), cData_->player_->GetSpeed());
		}
	}

	//スプライト
	cData_->titleSprite_->Update();

	//共通処理
	CommonUpdate();

	//シーン切り替え
	ChangeScene();
}

void TitleScene::Draw()
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

}

void TitleScene::DrawSprite()
{
	if (!cData_->performanceManager_->GetIsPerformance() && cData_->scene_ == TITLE) {
		cData_->titleSprite_->Draw(cData_->dxCommon_->GetCommandList());
	}

	//共通処理
	CommonDrawSprite();
}

void TitleScene::Finalize()
{
}

void TitleScene::ChangeScene()
{
	//左クリックしたら演出プラスシーンの初期化
	if (cData_->input_->IsMouseTrigger(LEFT_CLICK) && !cData_->performanceManager_->GetIsPerformance()) {
		cData_->performanceManager_->SetPerformanceNum(TITLETOPLAY);

		//プレイシーンは演出のため先に初期化をする
		PlaySceneInitialize();
	}

	//-----演出終了でのシーン切り替え-----
	if (cData_->scene_ != cData_->performanceManager_->GetIsChangeScene()) {

		//シーンを切り替え
		cData_->scene_ = cData_->performanceManager_->GetIsChangeScene();

		if (cData_->scene_ == PLAY) {
			//次シーンの生成
			BaseScene* scene = new GamePlayScene(cData_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
	}

}

void TitleScene::Collition()
{
}
