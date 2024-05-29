#include "ClearScene.h"

void ClearScene::Initialize()
{
}

void ClearScene::Update()
{
	//自機
	cData_->player_->UpdateClearScene();

	//ボス
	cData_->boss_->UpdateClearScene();

	//スプライト
	cData_->clearSprite_->Update();

	//共通処理
	CommonUpdate();

	//シーン切り替え
	ChangeScene();
}

void ClearScene::Draw()
{
	//共通処理
	CommonDraw();

	//自機
	cData_->player_->DrawClearScene(cData_->dxCommon_->GetCommandList());

	//ボス
	cData_->boss_->Draw(cData_->dxCommon_->GetCommandList());
}

void ClearScene::DrawSprite()
{
	cData_->clearSprite_->Draw(cData_->dxCommon_->GetCommandList());

	//共通処理
	CommonDrawSprite();
}

void ClearScene::Finalize()
{
}

void ClearScene::ChangeScene()
{
	//-----シーン切り替え時演出の開始-----
	//タイトルに戻る演出
	if (cData_->input_->IsMouseTrigger(LEFT_CLICK) && !cData_->performanceManager_->GetIsPerformance()) {
		cData_->performanceManager_->SetPerformanceNum(RETURNTITLE);
	}

	//-----演出終了でのシーン切り替え-----
	if (cData_->scene_ != cData_->performanceManager_->GetIsChangeScene()) {

		//シーンを切り替え
		cData_->scene_ = cData_->performanceManager_->GetIsChangeScene();

		if (cData_->scene_ == TITLE) {
			//次シーンの生成
			BaseScene* scene = new TitleScene(cData_,collisionManager_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
	}


}
