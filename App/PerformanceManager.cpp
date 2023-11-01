/**
* @file PerformanceManager.h
* @brief 演出系
*/

#include "PerformanceManager.h"

void PerformanceManager::Initialize(Camera* camera, Player* player)
{
	//カメラ
	this->camera_ = camera;
	//プレイヤー
	this->player_ = player;

	//スプライト
	for (int i = 0; i < generalPurposeSpritesSize_;i++) {
		Sprite *gPSprite = new Sprite();
		gPSprite->SetTextureNum(4);
		gPSprite->Initialize();
		gPSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
		gPSprite->SetScale(XMFLOAT2(window_width, window_height
		));

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
		case RETURNTITLE:
			ReturnTitlePerformance();
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
		case RETURNTITLE:
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
		case RETURNTITLE:
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
		//イージング用の開始位置と終了位置を取得

		//現在の座標を開始位置に
		titleToPlayStartEye_ = camera_->GetEye();
		titleToPlayStartTarget_ = camera_->GetTarget();

		//初期座標を保持
		titleToPlayHoldEye_ = titleToPlayStartEye_;
		titleToPlayHoldTarget_ = titleToPlayStartTarget_;

		//現在の描画最大距離を終了位置に
		titleToPlayEndEye_ = camera_->GetEye();
		titleToPlayEndTarget_ = camera_->GetTarget();

		titleToPlayEndEye_.z = camera_->GetEye().z + camera_->GetFarClip();
		titleToPlayEndTarget_.z = camera_->GetEye().z + camera_->GetFarClip() + camera_->GetplayerRange();

		//イージング用数値の初期化
		titleToPlayEaseing_.Start(titleToPlayEaseingTime_);
		titleToPlayPhase_ = 0;

		//パフォーマンスフラグ
		isPerformance_ = true;

		//カメラのモードを演出モードに
		camera_->SetMode(PERFORMANCEMODE);
	}

	if (titleToPlayPhase_ == 0) {

		//取得したイージング用の開始位置と終了位置でイージングを行う
		titleToPlayEye_ = EaseIn3D(titleToPlayStartEye_, titleToPlayEndEye_, titleToPlayEaseing_.timeRate);
		titleToPlayTarget_ = EaseIn3D(titleToPlayStartTarget_, titleToPlayEndTarget_, titleToPlayEaseing_.timeRate);

		if (!titleToPlayEaseing_.GetActive()) {
			//演出が終わったら次のフェーズへ
			titleToPlayPhase_++;

			//イージング用のデータを設定しなおす
			titleToPlayEaseing_.Start(titleToPlayEaseingTime_);

			titleToPlayStartEye_ = camera_->GetEye();;
			titleToPlayStartTarget_ = camera_->GetTarget();;

			titleToPlayEndEye_ = titleToPlayHoldEye_;
			titleToPlayEndTarget_ = titleToPlayHoldTarget_;
		}
	}
	else if (titleToPlayPhase_ == 1) {

		//取得したイージング用の開始位置と終了位置でイージングを行う
		titleToPlayEye_ = EaseIn3D(titleToPlayStartEye_, titleToPlayEndEye_, titleToPlayEaseing_.timeRate);
		titleToPlayTarget_ = EaseIn3D(titleToPlayStartTarget_, titleToPlayEndTarget_, titleToPlayEaseing_.timeRate);

		//演出終了
		if (!titleToPlayEaseing_.GetActive()) {
			isPerformance_ = false;
			//シーンをプレイシーンへ
			isChangeScene_ = PLAY;

			//カメラのモードを自機追従モードに
			camera_->SetMode(PLAYERFOLLOWMODE);
		}
	}

	//イージングの更新
	titleToPlayEaseing_.Update();

	//カメラに座標の引き渡し
	camera_->SetEye(titleToPlayEye_);
	camera_->SetTarget(titleToPlayTarget_);

}

void PerformanceManager::BossInPerformance()
{
	//シーンをボスシーンへ
	isChangeScene_ = BOSS;
	isPerformance_ = false;
}

void PerformanceManager::BossClearPerformance()
{
	//シーンをクリアーンへ
	isChangeScene_ = CLEAR;
	isPerformance_ = false;

}

void PerformanceManager::BossGameoverPerformance()
{

	//プレイヤーの座用、スピードを取得
	XMFLOAT3 playerPosition = player_->GetPosition();
	float playerSpeed = player_->GetSpeed();

	//降下終了地点まで降下したら終了
	if (downPosition_ <= playerPosition.y) {
		//降下させる
		playerPosition.y -= downSpeed_;

		//Zスピードの減少
		if (playerSpeed - subSpeed_ >= 0) {
			playerSpeed -= subSpeed_;
		}
		else {
			playerSpeed = 0;
		}

	}
	else {
		//演出の終了
		//シーンをゲームオーバーシーンへ
		isChangeScene_ = GAMEOVER;
		isPerformance_ = false;
	}

	player_->SetPosition(playerPosition);
	player_->SetSpeed(playerSpeed);

}

void PerformanceManager::ReturnTitlePerformance()
{
	//演出初期化
	if (startPerformance_) {
		//イージング用のデータを設定
		generalPurposeEaseing1_.Start(titleToPlayEaseingTime_);
		generalPurposeEaseing2_.Start(titleToPlayEaseingTime_);


		generalPurposeSpritePosition1_ = generalPurposeEaseStartPosition1_;
		generalPurposeSpritePosition2_ = generalPurposeEaseStartPosition2_;

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
		isChangeScene_ = TITLE;
	}

	//終わったら演出終了
	if (!generalPurposeEaseing1_.GetActive() && !generalPurposeEaseing2_.GetActive()) {
		isPerformance_ = false;
	}
}