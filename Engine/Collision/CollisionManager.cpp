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

		ReticleToEnemy2D();

		MenuUIColision();
		break;
	case BOSS:
		BossToPlayerBullet();
		PlayerToBossBullet();

		ReticleToBoss2D();

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

						//自機
						//弾を消す
						cData_->player_->SetBulletIsDead(true, i);
						//スピードを上げる
						cData_->player_->SpeedUp();

						//敵
						enemy->SetISDesd(true);

						//敵撃破パーティクル生成
						cData_->destroyParticle_->MakeParticle(enemy->GetPosition());
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

				//自機
				cData_->player_->CollEnemy();
				//敵
				enemy->SetISDesd(true);
				//敵撃破パーティクル生成
				cData_->destroyParticle_->MakeParticle(enemy->GetPosition());

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

						//自機
						cData_->player_->CollEnemy();

						//敵
						enemy->SetBulletIsDead(true, i);

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

					//自機
					//自機の弾を消す
					cData_->player_->SetBulletIsDead(true, i);

					//ボス
					//当たったらhpをへらす
					cData_->boss_->HitBullet();

					//弾着弾パーティクル
					cData_->landingParticle_->MakeParticle(cData_->player_->GetBulletPosition(i));

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

					//自機
					cData_->player_->CollBoss();

					//敵の弾を消す
					cData_->boss_->SetBulletIsDead(true, i);

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

void CollisionManager::ReticleToEnemy2D()
{
	//一番レティクルに近い敵番号
	int mostNearNum = 0;
	//一番レティクルに近い敵との距離
	float mostNearLeg = 10000.0f;

	//レティクル座標
	Vector2 rVec = { cData_->player_->GetReticlePosition().x,cData_->player_->GetReticlePosition().y };

	//イテレーター
	int it = 0;

	for (std::unique_ptr<Enemy>& enemy : cData_->enemys_)
	{

		//敵座標
		Vector2 eVec = { enemy->GetPosition2D().x, enemy->GetPosition2D().y };

		//レティクルと敵の距離
		Vector2 reticleToEnemyVec = eVec - rVec;
		float reticleToEnemyLeg = reticleToEnemyVec.length();

		//距離が一番近いか
		if (mostNearLeg > reticleToEnemyLeg) {
			//近かったら更新
			mostNearLeg = reticleToEnemyLeg;
			mostNearNum = it;
		}

		it++;

	}

	//プレイヤーの射撃z軸をセット
	if (cData_->enemys_.size() != 0) {
		float reticleDirection =
			cData_->player_->GetPlayerDirection() +
			(cData_->enemys_[mostNearNum]->GetPosition().z - cData_->player_->GetPosition().z);

		cData_->player_->SetReticleDirection_(reticleDirection);
	}
}

void CollisionManager::ReticleToBoss2D()
{
	//プレイヤーの射撃z軸をセット

	float reticleDirection =
		cData_->player_->GetPlayerDirection() +
		(cData_->boss_->GetPosition().z - cData_->player_->GetPosition().z);
	cData_->player_->SetReticleDirection_(reticleDirection);

}
