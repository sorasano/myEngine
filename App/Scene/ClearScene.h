/**
* @file ClearScene.h
* @brief　クリアシーン
*/


#pragma once
#include "BaseScene.h"

class ClearScene : public BaseScene
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
};

