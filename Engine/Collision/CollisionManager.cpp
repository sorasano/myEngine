#include "CollisionManager.h"

void CollisionManager::Initialize(SceneCommonData* cData)
{
	cData_ = cData;
}


void CollisionManager::Update()
{
	//シーンごとの当たり判定の更新
	switch (cData_->scene_)
	{
	case TITLE:
		break;
	case PLAY:
		EnemyToPlayerBullet();
		EnemyToPlayer();
		EnemyToEnemy();
		PlayerToEnemyBullet();

		MenuUIColision();
		break;
	case BOSS:
		BossToPlayerBullet();
		PlayerToBossBullet();

		MenuUIColision();
		break;
	case CLEAR:
		break;
	case GAMEOVER:
		break;
	case MENU:
		break;
	}

}

void CollisionManager::EnemyToPlayerBullet()
{
	if (cData_->player_->GetBulletSize() != 0) {

		for (int i = 0; i < cData_->player_->GetBulletSize(); i++) {

			for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
			{
				if (!enemy->GetIsDead()) {
					//当たったか
					if (Collision::CheckSquareToSquare(enemy->GetColData(), cData_->player_->GetBulletColData(i))) {

						//当たったら敵は消してパーティクル生成
						enemy->SetISDesd(true);
						//敵撃破パーティクル生成
						cData_->destroyParticle_->MakeParticle(enemy->GetPosition());

						//自機の弾を消し、自機のスピードを上げスコアを加算
						cData_->player_->SetBulletIsDead(true, i);
						cData_->player_->SpeedUpByEnemy();
						//弾着弾パーティクル
						cData_->landingParticle_->MakeParticle(cData_->player_->GetBulletPosition(i));

					}
				}
			}
		}
	}
}

void CollisionManager::EnemyToPlayer()
{
	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (!enemy->GetIsDead()) {
			//当たったか
			if (Collision::CheckSquareToSquare(enemy->GetColData(), cData_->player_->GetColData())) {

				//当たったら敵は消してパーティクル生成
				enemy->SetISDesd(true);
				//敵撃破パーティクル生成
				cData_->destroyParticle_->MakeParticle(enemy->GetPosition());

				//自機のスピードを下げ,少し無敵時間に
				cData_->player_->SpeedDownByEnemy();
				cData_->player_->SetIsInvincible(true);

			}
		}
	}

}

void CollisionManager::EnemyToEnemy()
{
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
				if (Collision::CheckSquareToSquare(it1->get()->GetColData(), it2->get()->GetColData())) {

					//当たったら反射させる
					it1->get()->Reflection();
					it2->get()->Reflection();

				}

			}

		}
	}
}

void CollisionManager::PlayerToEnemyBullet()
{

	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{
		if (enemy->GetBulletSize() != 0) {

			for (int i = 0; i < enemy->GetBulletSize(); i++) {

				if (!enemy->GetIsDead()) {
					//当たったか
					if (Collision::CheckSquareToSquare(cData_->player_->GetColData(), enemy->GetBulletColData(i))) {

						//当たったら敵の弾を消し、自機のスピードを下げげスコアを減算
						enemy->SetBulletIsDead(true, i);
						cData_->player_->SpeedDownByEnemy();
						//弾着弾パーティクル
						cData_->landingParticle_->MakeParticle(enemy->GetBulletPosition(i));

					}
				}
			}
		}
	}
}

void CollisionManager::BossToPlayerBullet()
{
		if (cData_->player_->GetBulletSize() != 0) {

		for (int i = 0; i < cData_->player_->GetBulletSize(); i++) {


			if (!cData_->boss_->GetIsDead()) {
				//当たったか
				if (Collision::CheckSquareToSquare(cData_->boss_->GetColData(), cData_->player_->GetBulletColData(i))) {

					//当たったらhpをへらす
					cData_->boss_->HitBullet();

					//自機の弾を消し、パーティクル生成
					cData_->player_->SetBulletIsDead(true, i);
					cData_->landingParticle_->MakeParticle(cData_->player_->GetBulletPosition(i));

					//スコアを加算
				}
			}

		}
	}
}

void CollisionManager::PlayerToBossBullet()
{	
	if (cData_->boss_->GetBulletSize() != 0) {

		for (int i = 0; i < cData_->boss_->GetBulletSize(); i++) {

			if (!cData_->boss_->GetIsDead()) {
				//当たったか
				if (Collision::CheckSquareToSquare(cData_->player_->GetColData(), cData_->boss_->GetBulletColData(i))) {

					//当たったら敵の弾を消し、自機のスピードを下げスコアを減算
					cData_->boss_->SetBulletIsDead(true, i);
					cData_->player_->SpeedDownByEnemy();
					//パーティクル生成
					cData_->landingParticle_->MakeParticle(cData_->boss_->GetBulletPosition(i));

				}
			}
		}
	}
}

bool CollisionManager::MenuUIColision()
{
	XMFLOAT2 mousePos = cData_->input_->GetMousePosition();

	if (Collision::CheckSpriteTo2Dpos(cData_->menuUISprite_.get(), mousePos)) {

		cData_->performanceManager_->MenuUIRotPerformance(cData_->menuUISprite_.get());

		if (cData_->input_->IsMouseTrigger(LEFT_CLICK)) {

			return true;
		}
	}

	return false;
}
