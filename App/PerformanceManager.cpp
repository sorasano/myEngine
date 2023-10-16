/**
* @file PerformanceManager.h
* @brief 演出系
*/

#include "PerformanceManager.h"

void PerformanceManager::Initialize(Camera* camera)
{
	//カメラ
	this->camera_ = camera;

	//スプライト
	for (int i = 0; i < generalPurposeSpritesSize_;i++) {
		Sprite *gPSprite = new Sprite();
		gPSprite->SetTextureNum(3);
		gPSprite->Initialize();
		gPSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
		gPSprite->SetScale(XMFLOAT2(window_width + 100, window_width));

		generalPurposeSprites_.push_back(gPSprite);
	}
}

void PerformanceManager::Update()
{

	//演出が始まったったタイミング取得
	if (isPerformance_ == true && oldIsPerformance_ == false) {
		startPerformance_ = true;
	}
	else {
		startPerformance_ = false;
	}

	oldIsPerformance_ = isPerformance_;

	if (isPerformance_) {

		switch (performanceNum_) {

		case TITLETOPLAY:
			TitleToPlayPerformance();
			break;
		case INBOSS:
			BossInPerformance();
			break;
		case CLEARBOSS:
			BossClearPerformance();
			break;
		case GAMEOVERBOSS:
			BossGameoverPerformance();
			break;
		case GENERALPURPOSE:
			GeneralpurposePerformance();
		}
	}

	//スプライト
	UpdateSprite();
}

void PerformanceManager::UpdateSprite()
{
	if (isPerformance_) {

		switch (performanceNum_) {

		case TITLETOPLAY:

			break;
		case INBOSS:

			break;
		case CLEARBOSS:

			break;
		case GAMEOVERBOSS:

			break;
		case GENERALPURPOSE:
			for (auto& gPSprite : generalPurposeSprites_) {
				gPSprite->Update();
			}
		}
	}

}

void PerformanceManager::DrawSprite(ID3D12GraphicsCommandList* cmdList)
{

	if (isPerformance_) {

		switch (performanceNum_) {

		case TITLETOPLAY:

			break;
		case INBOSS:

			break;
		case CLEARBOSS:

			break;
		case GAMEOVERBOSS:

			break;
		case GENERALPURPOSE:
			for (auto& gPSprite : generalPurposeSprites_) {
				gPSprite->Draw(cmdList);
			}
		}
	}
}

void PerformanceManager::TitleToPlayPerformance()
{
	//演出初期化
	if (startPerformance_) {
		//camera_->Update(player_->GetPosition(), boss_->GetPosition());
		//camera_->SetMode(TITLETOPLAYMODE);
	}

	//カメラ演出が終わったら演出終了
	if (!camera_->GetIsPerformance()) {
		isPerformance_ = false;
		isChangeScene_ = true;
	}
}

void PerformanceManager::BossInPerformance()
{
}

void PerformanceManager::BossClearPerformance()
{
}

void PerformanceManager::BossGameoverPerformance()
{
}

void PerformanceManager::GeneralpurposePerformance()
{
	//演出初期化
	if (startPerformance_) {
		//イージング用のデータを設定
		generalPurposeEaseing1_.Start(easeingTime_);
		generalPurposeEaseing2_.Start(easeingTime_);

	}

	//取得したイージング用の開始位置と終了位置でイージングを行う
	generalPurposeSpritePosition1_ = EaseIn2D(generalPurposeEaseStartPosition1_, generalPurposeEaseEndPosition1_, generalPurposeEaseing1_.timeRate);
	generalPurposeSpritePosition2_ = EaseIn2D(generalPurposeEaseStartPosition2_, generalPurposeEaseEndPosition2_, generalPurposeEaseing2_.timeRate);

	//座標をセット
	generalPurposeSprites_[0]->SetPosition(generalPurposeSpritePosition1_);
	generalPurposeSprites_[1]->SetPosition(generalPurposeSpritePosition2_);

	generalPurposeEaseing1_.Update();
	generalPurposeEaseing2_.Update();

	//スプライトが中心で重なった瞬間シーンチェンジ
	if (generalPurposeSpritePosition1_.x - generalPurposeSpritePosition2_.x >= 10) {
		isChangeScene_ = true;
	}

	//終わったら演出終了
	if (!generalPurposeEaseing1_.GetActive() && !generalPurposeEaseing2_.GetActive()) {
		isPerformance_ = false;
	}
}