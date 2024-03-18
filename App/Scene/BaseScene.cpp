#include "BaseScene.h"

BaseScene::BaseScene()
{
	CommonInitialize();
}

BaseScene::BaseScene(SceneCommonData* cData)
{
	cData_ = cData;
}

void BaseScene::CommonInitialize()
{
	//共通変数初期化
	SceneCommonData* newCData = new SceneCommonData;
	cData_ = newCData;
}

void BaseScene::CommonUpdate()
{
	//カメラ更新
	cData_->camera_->Update(cData_->player_->GetPosition());

	//背景
	UpdateBackGround();

	//演出
	cData_->performanceManager_->Update();

	//パーティクル更新
	cData_->particleManager_->Update();

	//パーティクルマネージャー静的更新
	ParticleManager::StaticUpdate(cData_->camera_->GetEye(), cData_->camera_->GetTarget());
}

void BaseScene::CommonDraw()
{
	//スカイドーム
	cData_->skydome_->Draw(cData_->dxCommon_->GetCommandList());

	//背景
	for (std::unique_ptr<BackGround>& backGround : cData_->backGrounds_) {
		if (UpadateRange(cData_->camera_->GetEye(), backGround->GetPosition())) {
			backGround->Draw(cData_->dxCommon_->GetCommandList());
		}
	}
}

void BaseScene::CommonDrawSprite()
{
	//演出
	cData_->performanceManager_->DrawSprite(cData_->dxCommon_->GetCommandList());

	//パーティクル
	cData_->particleManager_->Draw();
}

void BaseScene::PlaySceneInitialize()
{
	//プレイヤー
	cData_->player_->SetPosition(XMFLOAT3{ cData_->camera_->GetEye().x,cData_->camera_->GetEye().y - 10,cData_->camera_->GetEye().z });

	//敵
	SetEnemy();
	cData_->phase_ = 1;
}

void BaseScene::SetEnemy()
{
	//発生させる位置はスカイドームの端
	float makePos = cData_->player_->GetPosition().z + cData_->skydome_->GetEdge();

	//何番目のCSVをセットするか(ランダム)
	int setNum = static_cast<int>(Random(0, cData_->enemyCSVSize_ - 0.001f));
	auto it = cData_->enemyCsvs_.begin();
	std::advance(it, setNum);

	for (int i = 0; i < it->get()->GetSize(); i++)
	{
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(cData_->enemyModel_.get(), cData_->enemyBulletModel_.get());

		if (it->get()->GetType(i) == 1 || it->get()->GetType(i) == 2) {
			newObject->SetModel(cData_->enemyBlueModel_.get());
		}
		else if (it->get()->GetType(i) > 2) {
			newObject->SetModel(cData_->enemyYellowModel_.get());
		}

		newObject->SetPosition(XMFLOAT3(it->get()->GetPosition(i).x, it->get()->GetPosition(i).y, it->get()->GetPosition(i).z + makePos));
		newObject->SetType(it->get()->GetType(i));
		newObject->SetStopInScreen(it->get()->GetStopInScreen(i));

		cData_->enemys_.push_back(std::move(newObject));
	}
}

bool BaseScene::UpadateRange(const XMFLOAT3& cameraPos, const XMFLOAT3& pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	//スカイドームまでを更新描画
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -cData_->skydome_->GetEdge()) { return true; }

	return false;
}

void BaseScene::UpdateBackGround()
{
	//背景
	for (std::unique_ptr<BackGround>& backGround : cData_->backGrounds_)
	{
		backGround->Update();

		//背景の位置をカメラが通り過ぎたら
		if (backGround->GetPosition().z + backGround->GetSize() * 2.5 < cData_->camera_->GetEye().z) {

			//過ぎたオブジェクトを削除
			backGround->DeleteObject();

			//オブジェクトを配置しなおす
			backGround->SetObject(cData_->adjustPos_);

			//現在の位置+1つ分のサイズで次のマップの位置にセット
			cData_->adjustPos_ = backGround->GetPosition().z + backGround->GetSize();

		}

	}

	//スカイドーム
	cData_->skydome_->Update(cData_->camera_->GetEye().z);
}

bool BaseScene::MenuUIColision()
{
	XMFLOAT2 mousePos = cData_->input_->GetMousePosition();

	if (cData_->collisionManager_->CheckSpriteTo2Dpos(cData_->menuUISprite_.get(), mousePos)) {

		cData_->performanceManager_->MenuUIRotPerformance(cData_->menuUISprite_.get());

		if (cData_->input_->IsMouseTrigger(LEFT_CLICK)) {

			return true;
		}
	}

	return false;
}

void BaseScene::Reset()
{
	//プレイヤー
	cData_->player_->Reset();

	//敵
	cData_->enemys_.clear();
	cData_->phase_ = 0;

	//ボス
	cData_->boss_->Reset();

	//カメラ
	cData_->camera_->Reset();

	cData_->menu_->Reset();


	//背景
	//オブジェクトを全削除
	cData_->backGrounds_.clear();

	//オブジェクトを配置しなおす
	cData_->adjustPos_ = 0;
	for (int i = 0; i < cData_->backGroundSize_; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(cData_->adjustPos_);
		//現在の位置+1つ分のサイズで次のマップの位置にセット
		cData_->adjustPos_ = newBackGround->GetPosition().z + newBackGround->GetSize();
		cData_->backGrounds_.push_back(std::move(newBackGround));
	}
}

