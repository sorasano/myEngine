﻿/**
* @file TitleScene.h
* @brief　タイトルシーン
*/

#pragma once
#include "SceneManager.h"
#include "BaseScene.h"
#include "GamePlayScene.h"

class TitleScene : public BaseScene
{
public:

	TitleScene() : BaseScene() {};

	TitleScene(SceneCommonData* cData, CollisionManager* collisionManager) : BaseScene(cData, collisionManager) {};

	/**
	* 初期化
	*
	*/
	void Initialize() override;
	/**
	* 更新
	*/
	void Update() override;
	/**
	* 描画
	*/
	void Draw() override;
	/**
	* スプライト描画
	*/
	void DrawSprite() override;
	/**
	* 終了処理
	*/
	void Finalize() override;

private:

	/**
	* シーン切り替え
	*/
	void ChangeScene() override;

};

