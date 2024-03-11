#include "MenuScene.h"

void MenuScene::Initialize() {

}

void MenuScene::Update()
{
	cData_->menu_->Update();

	//共通処理
	CommonUpdate();

	//シーン切り替え
	ChangeScene();
}

void MenuScene::Draw()
{

	//共通処理
	CommonDraw();

	//自機
	cData_->player_->Draw(cData_->dxCommon_->GetCommandList());

	//敵
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (UpadateRange(cData_->camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(cData_->dxCommon_->GetCommandList());
		}
	}

	//ボス
	cData_->boss_->Draw(cData_->dxCommon_->GetCommandList());
}

void MenuScene::DrawSprite()
{
	cData_->menu_->Draw(cData_->dxCommon_->GetCommandList());

	//共通処理
	CommonDrawSprite();
}

void MenuScene::Finalize()
{
}

void MenuScene::ChangeScene()
{
	//メニューでシーン切り替えフラグがたったら
	if (cData_->menu_->GetIsSerect() && !cData_->performanceManager_->GetIsPerformance()) {

		//選択中の物を参照してシーン遷移
		if (cData_->menu_->GetSerect() == MENUTITLE) {
			//タイトルに戻る演出
			cData_->performanceManager_->SetPerformanceNum(RETURNTITLE);
		}
		else if (cData_->menu_->GetSerect() == MENUCLOSE) {
			//メニューを閉じる
			cData_->performanceManager_->SetPerformanceNum(CLOSEMENU);
		}

		//メニューを閉じたとき用リセット
		cData_->menu_->CloseReset();

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
		else if (cData_->scene_ == BOSS) {
			//次シーンの生成
			BaseScene* scene = new BossScene(cData_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}else if (cData_->scene_ == TITLE) {
			//次シーンの生成
			BaseScene* scene = new TitleScene(cData_);
			//シーン切り替え依頼
			sceneManager_->SetNextScene(scene);
		}
	}
}

void MenuScene::Collition()
{
}
