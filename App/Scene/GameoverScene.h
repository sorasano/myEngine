/**
* @file GameoverScene.h
* @brief　ゲームオーバーシーン
*/

#pragma once
#include "SceneManager.h"
#include "BaseScene.h"

class GameoverScene : public BaseScene
{
public:
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

	/**
	* シーン切り替え
	*/
	void ChangeScene() override;
	/**
	* 当たり判定
	*/
	void Collition() override;
};

