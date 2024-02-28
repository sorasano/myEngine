#include "TitleScene.h"

void TitleScene::Initialize()
{
}

void TitleScene::Update()
{
	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Update(player_->GetPosition(), player_->GetSpeed());
		}
	}

	//スプライト
	titleSprite_->Update();

	//共通処理
	CommonUpdate();
}

void TitleScene::Draw()
{
	//共通処理
	CommonDraw();

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}

}

void TitleScene::DrawSprite()
{
	if (!performanceManager_->GetIsPerformance()) {
		titleSprite_->Draw(dxCommon_->GetCommandList());
	}

	//共通処理
	CommonDrawSprite();
}

void TitleScene::Finalize()
{
}
