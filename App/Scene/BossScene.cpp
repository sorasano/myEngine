#include "BossScene.h"

void BossScene::Initialize()
{
	cData_->scene_ = BOSS;
}

void BossScene::Update()
{
	//シーン切り替え
	ChangeScene();
}

void BossScene::Draw()
{
}

void BossScene::DrawSprite()
{
}

void BossScene::Finalize()
{
}

void BossScene::ChangeScene()
{
}

void BossScene::Collition()
{
}
