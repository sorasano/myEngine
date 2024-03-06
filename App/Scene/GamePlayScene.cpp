#include "GamePlayScene.h"

void GamePlayScene::Initialize()
{
	cData_->scene_ = PLAY;
}

void GamePlayScene::Update()
{
	//自機
	cData_->player_->Update(cData_->camera_->GetViewProjection());

	//敵
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (UpadateRange(cData_->camera_->GetEye(), enemy->GetPosition())) {
			enemy->Update(cData_->player_->GetPosition(), cData_->player_->GetSpeed());
		}
	}

	//当たり判定
	Collition();

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
	if (MenuUIColision()) {
		cData_->performanceManager_->SetIsOldScene(PLAY);
		cData_->performanceManager_->SetPerformanceNum(OPENMENU);
	}
}

void GamePlayScene::Collition()
{
#pragma region 敵と自機の弾の当たり判定

	if (cData_->player_->GetBulletSize() != 0) {

		for (int i = 0; i < cData_->player_->GetBulletSize(); i++) {

			for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
			{
				if (!enemy->GetIsDead()) {
					//当たったか
					if (cData_->collisionManager_->CheckSquareToSquare(enemy->GetColData(), cData_->player_->GetBulletColData(i))) {

						//当たったら敵は消してパーティクル生成
						enemy->SetISDesd(true);
						//敵撃破パーティクル生成
						cData_->particleManager_->MakeParticle(ENEMYDESTROY, enemy->GetPosition());

						//自機の弾を消し、自機のスピードを上げスコアを加算
						cData_->player_->SetBulletIsDead(true, i);
						cData_->player_->SpeedUpByEnemy();

					}
				}
			}
		}
	}

#pragma endregion 

#pragma region	敵と自機の当たり判定

	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (!enemy->GetIsDead()) {
			//当たったか
			if (cData_->collisionManager_->CheckSquareToSquare(enemy->GetColData(), cData_->player_->GetColData())) {

				//当たったら敵は消してパーティクル生成
				enemy->SetISDesd(true);
				//敵撃破パーティクル生成
				cData_->particleManager_->MakeParticle(ENEMYDESTROY, enemy->GetPosition());

				//自機のスピードを下げ,少し無敵時間に
				cData_->player_->SpeedDownByEnemy();
				cData_->player_->SetIsInvincible(true);

			}
		}
	}

#pragma endregion 

#pragma region	敵と敵

	const int enemySize = static_cast<const int>(cData_->enemys_.size());

	for (int i = 0; i < enemySize; i++) {
		for (int j = 0; j < enemySize; j++) {

			if (i < j) {
				break;
			}

			auto it1 = cData_->enemys_.begin();
			std::advance(it1, i);

			auto it2 = cData_->enemys_.begin();
			std::advance(it2, j);

			if (!it1->get()->GetIsDead() && !it2->get()->GetIsDead()) {

				//当たったか
				if (cData_->collisionManager_->CheckSquareToSquare(it1->get()->GetColData(), it2->get()->GetColData())) {

					//当たったら反射させる
					it1->get()->Reflection();
					it2->get()->Reflection();

				}

			}

		}
	}

#pragma endregion 

#pragma region 自機と敵の弾

	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (enemy->GetBulletSize() != 0) {

			for (int i = 0; i < enemy->GetBulletSize(); i++) {

				if (!enemy->GetIsDead()) {
					//当たったか
					if (cData_->collisionManager_->CheckSquareToSquare(cData_->player_->GetColData(), enemy->GetBulletColData(i))) {

						//当たったら敵の弾を消し、自機のスピードを下げげスコアを減算
						enemy->SetBulletIsDead(true, i);
						cData_->player_->SpeedDownByEnemy();

					}
				}
			}
		}
	}

#pragma endregion 

}

void GamePlayScene::CheckEnemy()
{
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_) {

		//カメラより後ろに行ったら死亡
		if (enemy->GetPosition().z < cData_->camera_->GetEye().z) {
			enemy->SetISDesd(true);
		}

	}

	//デスフラグがtrueであればリストから削除
	cData_->enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->GetIsDead(); });

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
