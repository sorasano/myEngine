#include "GameoverScene.h"

void GameoverScene::Initialize()
{
	cData_->scene_ = GAMEOVER;
}

void GameoverScene::Update()
{
	//シーン切り替え
	ChangeScene();
}

void GameoverScene::Draw()
{
}

void GameoverScene::DrawSprite()
{
}

void GameoverScene::Finalize()
{
}

void GameoverScene::ChangeScene()
{
}

void GameoverScene::Collition()
{
}
