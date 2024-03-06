#include "BossScene.h"

void BossScene::Initialize()
{
	cData_->scene_ = BOSS;
	cData_->boss_->SetPosition(XMFLOAT3{ 0,0,cData_->player_->GetPosition().z + 26.0f });
}

void BossScene::Update()
{
	//自機
	cData_->player_->Update(cData_->camera_->GetViewProjection());

	//ボス
	cData_->boss_->Update(cData_->player_->GetPosition(), cData_->player_->GetSpeed());

	//当たり判定
	Collition();

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
	//ボスが死んだらクリア
	if (cData_->boss_->GetIsDead()) {
		cData_->performanceManager_->SetPerformanceNum(CLEARBOSS);
	}

	//プレイヤーのスピードが0になったらゲームオーバー
	else if (!cData_->player_->GetAddSpeed()) {
		cData_->performanceManager_->SetPerformanceNum(GAMEOVERBOSS);
	}

	//メニュー
	if (MenuUIColision()) {
		cData_->performanceManager_->SetIsOldScene(BOSS);
		cData_->performanceManager_->SetPerformanceNum(OPENMENU);

	}
}

void BossScene::Collition()
{
#pragma region ボスと自機の弾の当たり判定

	if (cData_->player_->GetBulletSize() != 0) {

		for (int i = 0; i < cData_->player_->GetBulletSize(); i++) {


			if (!cData_->boss_->GetIsDead()) {
				//当たったか
				if (cData_->collisionManager_->CheckSquareToSquare(cData_->boss_->GetColData(), cData_->player_->GetBulletColData(i))) {

					//当たったらhpをへらす
					cData_->boss_->HitBullet();

					//自機の弾を消し、パーティクル生成
					cData_->player_->SetBulletIsDead(true, i);
					cData_->particleManager_->MakeParticle(PLAYERBULLETLANDING, cData_->player_->GetBulletPosition(i));

					//スコアを加算
				}
			}

		}
	}

#pragma endregion 

#pragma region 自機とボスの弾


	if (cData_->boss_->GetBulletSize() != 0) {

		for (int i = 0; i < cData_->boss_->GetBulletSize(); i++) {

			if (!cData_->boss_->GetIsDead()) {
				//当たったか
				if (cData_->collisionManager_->CheckSquareToSquare(cData_->player_->GetColData(), cData_->boss_->GetBulletColData(i))) {

					//当たったら敵の弾を消し、自機のスピードを下げスコアを減算
					cData_->boss_->SetBulletIsDead(true, i);
					cData_->player_->SpeedDownByEnemy();

				}
			}
		}
	}


#pragma endregion 
}
