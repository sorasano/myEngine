/**
* @file PerformanceManager.h
* @brief 演出系
*/

#include "PerformanceManager.h"
#include "Vector2.h"

void PerformanceManager::Initialize(Camera* camera, Player* player, Boss* boss)
{
	//カメラ
	this->camera_ = camera;
	//プレイヤー
	this->player_ = player;
	//ボス
	this->boss_ = boss;

	//スプライト
	for (int i = 0; i < titleReturnSpritesSize_; i++) {
		Sprite* gPSprite = new Sprite();
		gPSprite->SetTextureNum(4);
		gPSprite->Initialize();
		gPSprite->SetScale(titleReturnSpriteSize);

		titleReturnSprites_.push_back(gPSprite);
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
			break;
		case OPENMENU:
			OpenMenuPerformance();
			break;

		case CLOSEMENU:
			CloseMenuPerformance();
			break;
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
			for (auto& gPSprite : titleReturnSprites_) {
				gPSprite->Update();
			}
			break;
		case OPENMENU:
			break;

		case CLOSEMENU:
			break;
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
			for (auto& gPSprite : titleReturnSprites_) {
				gPSprite->Draw(cmdList);
			}
			break;
		case OPENMENU:
			break;

		case CLOSEMENU:
			break;
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

	//プレイヤーの座用、スピードを取得
	XMFLOAT3 playerPosition = player_->GetPosition();
	float playerSpeed = player_->GetSpeed();
	//ボスの座標を取得
	XMFLOAT3 bossPosition = boss_->GetPosition();
	//演出終了後プレイヤーの位置設定用
	const XMFLOAT3 midPosition = { 0.0f,0.0f,0.0f };

	//演出初期化
	if (startPerformance_) {
		//カメラのモードを演出モードに
		camera_->SetMode(PERFORMANCEMODE);

		clearPhase_ = CP_BOSSMOVE;

		//ボスの追従スピード
		clearBossSpeed_ = playerSpeed;

	}

	switch (clearPhase_)
	{
	case CP_BOSSMOVE://ボス墜落

		//降下終了地点まで降下したら終了
		if (clearDownPosition_ <= bossPosition.y) {
			//降下させる
			bossPosition.y -= clearDownSpeed_;
		}
		else {
			//フェーズの移行
			clearPhase_ = CP_PLAYERMOVE;
		}

		//Zスピードの減少
		if (clearBossSpeed_ - clearSubSpeed_ >= 0) {
			clearBossSpeed_ -= clearSubSpeed_;
		}
		else {
			clearBossSpeed_ = 0;
		}

		bossPosition.z += clearBossSpeed_;

		boss_->SetPosition(bossPosition);

		//カメラはボスを追従
		camera_->SetEye(playerPosition);
		camera_->SetTarget(bossPosition);

		break;

	case CP_PLAYERMOVE://プレイヤー中心移動

		//プレイヤー中心移動
		playerPosition.x = midPosition.x;
		playerPosition.y = midPosition.y;

		player_->SetPosition(playerPosition);
		//カメラのモードを自機旋回モードに
		camera_->SetMode(PLAYERTURNINGMODE);
		//フェーズの移行
		clearPhase_ = CP_END;

		break;

	case CP_END://演出終了
		//シーンをクリアーンへ
		isChangeScene_ = CLEAR;
		isPerformance_ = false;
		break;
	}

}

void PerformanceManager::BossGameoverPerformance()
{

	//演出初期化
	if (startPerformance_) {
		//プレイヤーの操作をロック
		player_->SetIsLockOperation(true);
		//プレイヤーの座標、スピードを取得
		gameoverPlayerPosition = player_->GetPosition();
		gameoverPlayerSpeed = player_->GetSpeed();

		//イージング用のデータを設定
		gemeoverDownEaseing_.Start(gemeoverEaseingTime_);
		gemeoverDownEaseStartPosition_ = player_->GetPosition();
		gemeoverDownEaseEndPosition_ = player_->GetPosition();
		gemeoverDownEaseEndPosition_.y = gemeoverDownPosition_;

		gameoverPhase_ = GP_PLAYERMOVE;
	}


	switch (gameoverPhase_) {
	case GP_PLAYERMOVE:

		//降下させる
		gameoverPlayerPosition = EaseIn3D(gemeoverDownEaseStartPosition_, gemeoverDownEaseEndPosition_, gemeoverDownEaseing_.timeRate);
		gemeoverDownEaseing_.Update();

		//スピードダウンさせる
		gameoverPlayerSpeed -= gameoverPlayerDownSpeed;

		//座標とスピードのセット
		player_->SetPosition(gameoverPlayerPosition);
		player_->SetSpeed(gameoverPlayerSpeed);

		//降下終了地点まで降下したらフェーズ移行
		if (!gemeoverDownEaseing_.GetActive()) {
			gameoverPhase_ = GP_CAMERAMOVE;

			//ぷれいやーのスピードを0に
			player_->SetSpeed(0.0f);

			//-----スプライン用のデータを設定(eye)-----
			gemeoverEyeSpline_.Start(gemeoverEyeSplineTime_);


			//始点と終点
			Vector3 start = { camera_->GetEye().x,camera_->GetEye().y,camera_->GetEye().z };
			Vector3 end = { player_->GetPosition().x,player_->GetPosition().y - 0.5f,player_->GetPosition().z + 15 };

			//中継点
			Vector3 p2;
			Vector3 p3;

			//カメラが壁にめり込まないようにプレイヤーの位置で右回り、左回りの変更
			if (player_->GetPosition().x > window_width / 2) {
				//左回り
				p2 = { start.x + 5,start.y - 4,start.z + 15 };
				p3 = { start.x + 8,start.y - 8,start.z + 30 };

				end.x -= 10;

			}
			else {
				//右周り
				p2 = { start.x - 5,start.y - 4,start.z + 15 };
				p3 = { start.x - 8,start.y - 8,start.z + 30 };

				end.x -= 10;
			}

			//配列に格納
			gemeoverEyeSplinePoints_ = { start,start,p2,p3,end,end };

			//1からスタートする
			gemeoverEyeStartIndex_ = 1;

			//-----スプライン用のデータを設定(target)-----
			gemeoverTargetSpline_.Start(gemeoverTargetSplineTime_);


			//始点と終点
			start = { camera_->GetTarget().x,camera_->GetTarget().y,camera_->GetTarget().z };
			end = { player_->GetPosition().x,player_->GetPosition().y,player_->GetPosition().z };

			//配列に格納
			gemeoverTargetSplinePoints_ = { start,start,end,end };

			//1からスタートする
			gemeoverTargetStartIndex_ = 1;

			//カメラのモードを演出モードに
			camera_->SetMode(PERFORMANCEMODE);

		}

		break;

	case GP_CAMERAMOVE:

		//-------スプライン(eye)-------

		//移動させる
		gemeoverSplineEye_ = Spline3D(gemeoverEyeSplinePoints_, gemeoverEyeStartIndex_, gemeoverEyeSpline_.timeRate);
		gameoverCameraEye = { gemeoverSplineEye_.x,gemeoverSplineEye_.y,gemeoverSplineEye_.z };

		//スプライン更新
		gemeoverEyeSpline_.Update();

		//カメラeyeセット
		camera_->SetEye(gameoverCameraEye);

		//-------スプライン(target)-------


		gemeoverSplineTarget_ = Spline3D(gemeoverTargetSplinePoints_, gemeoverTargetStartIndex_, gemeoverTargetSpline_.timeRate);
		gameoverCameraTarget = { gemeoverSplineTarget_.x,gemeoverSplineTarget_.y,gemeoverSplineTarget_.z };

		//スプライン更新
		gemeoverTargetSpline_.Update();

		//カメラtargetセット
		camera_->SetTarget(gameoverCameraTarget);



		if (!gemeoverEyeSpline_.GetActive()) {

			//startIndexが指定したポイントに移動しきるまでポイント移動
			if (gemeoverEyeStartIndex_ < gemeoverEyeSplinePoints_.size() - 3) {
				//移動終了で次ポイント移行
				gemeoverEyeStartIndex_++;
				//timerateリセット
				gemeoverEyeSpline_.Start(gemeoverEyeSplineTime_);
			}
			else {
				//移動終了でフェーズ移行
				gameoverPhase_ = GP_END;
			}
		}


		break;

	case GP_END:
		//演出の終了
		//シーンをゲームオーバーシーンへ
		isChangeScene_ = GAMEOVER;
		isPerformance_ = false;

		//プレイヤーの操作をロック解除
		player_->SetIsLockOperation(false);
	}

}

void PerformanceManager::ReturnTitlePerformance()
{
	//演出初期化
	if (startPerformance_) {
		//イージング用のデータを設定
		titleReturnCloseEaseing1_.Start(titleReturnCloseEaseingTime_);
		titleReturnCloseEaseing2_.Start(titleReturnCloseEaseingTime_);

		titleReturnPhase_ = TP_CLOSE;
	}

	switch (titleReturnPhase_) {

	case TP_CLOSE:

		//取得したイージング用の開始位置と終了位置でイージングを行う
		titleReturnSpritePosition1_ = EaseIn2D(titleReturnCloseEaseStartPosition1_, titleReturnCloseEaseEndPosition1_, titleReturnCloseEaseing1_.timeRate);
		titleReturnSpritePosition2_ = EaseIn2D(titleReturnCloseEaseStartPosition2_, titleReturnCloseEaseEndPosition2_, titleReturnCloseEaseing2_.timeRate);

		//座標をセット
		titleReturnSprites_[0]->SetPosition(titleReturnSpritePosition1_);
		titleReturnSprites_[1]->SetPosition(titleReturnSpritePosition2_);

		titleReturnCloseEaseing1_.Update();
		titleReturnCloseEaseing2_.Update();

		//移動終了で次フェーズ、タイトルシーンへ
		if (!titleReturnCloseEaseing1_.GetActive() && !titleReturnCloseEaseing2_.GetActive()) {
			isChangeScene_ = TITLE;
			titleReturnPhase_ = TP_OPEN;

			isTitleReturnOpen = true;
		}

		break;

	case TP_OPEN:

		if (isTitleReturnOpen) {

			//イージング用のデータを再設定
			titleReturnOpenEaseing1_.Start(titleReturnOpenEaseingTime_);
			titleReturnOpenEaseing2_.Start(titleReturnOpenEaseingTime_);

			isTitleReturnOpen = false;
		}

		//取得したイージング用の開始位置と終了位置でイージングを行う
		titleReturnSpritePosition1_ = EaseIn2D(titleReturnOpenEaseStartPosition1_, titleReturnOpenEaseEndPosition1_, titleReturnOpenEaseing1_.timeRate);
		titleReturnSpritePosition2_ = EaseIn2D(titleReturnOpenEaseStartPosition2_, titleReturnOpenEaseEndPosition2_, titleReturnOpenEaseing2_.timeRate);

		//座標をセット
		titleReturnSprites_[0]->SetPosition(titleReturnSpritePosition1_);
		titleReturnSprites_[1]->SetPosition(titleReturnSpritePosition2_);

		titleReturnOpenEaseing1_.Update();
		titleReturnOpenEaseing2_.Update();

		//移動終了で次フェーズ
		if (!titleReturnOpenEaseing1_.GetActive() && !titleReturnOpenEaseing2_.GetActive()) {
			titleReturnPhase_ = TP_END;
		}

		break;

	case TP_END:

		isPerformance_ = false;

		break;
	}

}

void PerformanceManager::OpenMenuPerformance()
{
	//シーンをメニューシーンへ
	isChangeScene_ = MENU;
	isPerformance_ = false;
}

void PerformanceManager::CloseMenuPerformance()
{
	//シーンを開いた時のシーンへ
	isChangeScene_ = oldScene_;
	isPerformance_ = false;
}

void PerformanceManager::MenuUIRotPerformance(Sprite* menuUI)
{
	//度数の加算
	menuUIRot += menuUIAddRot;
	if (menuUIRot >= 360.0f) {
		menuUIRot = 0.0f;
	}

	menuUI->SetRotation(static_cast<float>(menuUIRot * (PI / 180)));
}
