/**
* @file InitScene.h
* @brief　初期化シーン
*/

#pragma once
#include "SceneManager.h"
#include "TitleScene.h"

class InitScene :
    public BaseScene
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

