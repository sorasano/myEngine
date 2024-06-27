/**
* @file TitleScene.h
* @brief　シーンの切り替え、実行
*/

#pragma once
#include "SceneManager.h"
#include "BaseScene.h"

class SceneManager
{
public:
	/**
	* デストラクタ
	*/
	~SceneManager();

	/**
	* 次シーン予約
	*/
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }

	/**
	* 更新
	*/
	void Update();
	/**
	* 描画
	*/
	void Draw();
	/**
	* スプライト描画
	*/
	void DrawSprite();

	/**
	* ブラー要求フラグ
	* 
	* return bool playerのスピードアップフラグ
	*/
	bool GetIsBlur() { return scene_->GetSceneCommonData()->player_->GetIsSpeedUp(); }

private:
	//今のシーン(実行中シーン)
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;
};

