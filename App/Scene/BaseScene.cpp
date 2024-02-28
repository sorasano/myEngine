#include "BaseScene.h"

void BaseScene::CommonUpdate()
{
	//カメラ更新
	camera_->Update(player_->GetPosition());

	//背景
	UpdateBackGround();

	//演出
	performanceManager_->Update();

	//パーティクル更新
	particleManager_->Update();

	//パーティクルマネージャー静的更新
	ParticleManager::StaticUpdate(camera_->GetEye(), camera_->GetTarget());
}

void BaseScene::CommonDraw()
{
	//スカイドーム
	skydome_->Draw(dxCommon_->GetCommandList());

	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_) {
		if (UpadateRange(camera_->GetEye(), backGround->GetPosition())) {
			backGround->Draw(dxCommon_->GetCommandList());
		}
	}
}

void BaseScene::CommonDrawSprite()
{
	//演出

	performanceManager_->DrawSprite(dxCommon_->GetCommandList());

	//パーティクル
	particleManager_->Draw();
}

bool BaseScene::UpadateRange(const XMFLOAT3& cameraPos, const XMFLOAT3& pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	//スカイドームまでを更新描画
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -skydome_->GetEdge()) { return true; }

	return false;
}

void BaseScene::UpdateBackGround()
{
	//背景
	for (std::unique_ptr<BackGround>& backGround : backGrounds_)
	{
		backGround->Update();

		//背景の位置をカメラが通り過ぎたら
		if (backGround->GetPosition().z + backGround->GetSize() * 2.5 < camera_->GetEye().z) {

			//過ぎたオブジェクトを削除
			backGround->DeleteObject();

			//オブジェクトを配置しなおす
			backGround->SetObject(adjustPos_);

			//現在の位置+1つ分のサイズで次のマップの位置にセット
			adjustPos_ = backGround->GetPosition().z + backGround->GetSize();

		}

	}

	//スカイドーム
	skydome_->Update(camera_->GetEye().z);
}

