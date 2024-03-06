#include "ClearScene.h"

void ClearScene::Initialize()
{
	cData_->scene_ = CLEAR;
}

void ClearScene::Update()
{
	//シーン切り替え
	ChangeScene();
}

void ClearScene::Draw()
{
}

void ClearScene::DrawSprite()
{
}

void ClearScene::Finalize()
{
}

void ClearScene::ChangeScene()
{
}

void ClearScene::Collition()
{
}
