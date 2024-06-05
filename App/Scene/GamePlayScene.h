/**
* @file GamePlayScene.h
* @brief　プレイシーン
*/

#pragma once
#include "SceneManager.h"
#include "BaseScene.h"

#include "GameoverScene.h"
#include "BossScene.h"
#include "MenuScene.h"

class GamePlayScene : public BaseScene
{
public:
	GamePlayScene(SceneCommonData* cData, CollisionManager* collisionManager) : BaseScene(cData, collisionManager) {};

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
	 シーン切り替え
	*/
	void ChangeScene() override;

	/**
	* 敵生存確認
	*/
	void CheckEnemy();

	/**
	* 敵削除処理
	*/
	void DeleteEnemy();
};

