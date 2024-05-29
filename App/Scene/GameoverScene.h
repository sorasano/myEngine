/**
* @file GameoverScene.h
* @brief　ゲームオーバーシーン
*/

#pragma once
#include "SceneManager.h"
#include "BaseScene.h"

#include "TitleScene.h"

class GameoverScene : public BaseScene
{

public:
	GameoverScene(SceneCommonData* cData, CollisionManager* collisionManager) : BaseScene(cData, collisionManager) {};

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

