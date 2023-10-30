/**
* @file PerformanceManager.h
* @brief 演出系
*/

#pragma once

#include "Scene.h"
#include "Camera.h"
#include "Sprite.h"
#include "Easing.h"
#include "Player.h"

enum Performance {
	TITLETOPLAY,//タイトル→プレイシーン遷移
	INBOSS,//ボス戦導入
	CLEARBOSS,//ボス戦クリア
	GAMEOVERBOSS,//ボス戦ゲームオーバー
	RETURNTITLE//タイトルに戻るとき
};

class PerformanceManager
{
public:
	/**
	* 初期化
	*/
	void Initialize(Camera* camera,Player* player);
	/**
	* 更新
	*/
	void Update();
	/**
	* スプライト更新
	*/
	void UpdateSprite();
	/**
	* スプライト描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void DrawSprite(ID3D12GraphicsCommandList* cmdList);

	//演出
	/**
	* タイトル→プレイシーン遷移演出
	*/
	void TitleToPlayPerformance();
	/**
	* ボス戦導入演出
	*/
	void BossInPerformance();
	/**
	* ボス撃破演出
	*/
	void BossClearPerformance();
	/**
	* ボス逃亡演出
	*/
	void BossGameoverPerformance();
	/**
	* タイトルに戻る演出
	*/
	void ReturnTitlePerformance();


	//ゲッター　セッター　
	/**
	* @return bool isPerformance_取得
	*/
	bool GetIsPerformance() { return isPerformance_; };
	/**
	* @return int isChangeScene取得
	*/
	int GetIsChangeScene() { return isChangeScene_; };

	/**
	* performanceNumセット
	*/
	void SetPerformanceNum(int performanceNum) { this->performanceNum_ = performanceNum, isPerformance_ = true; }
	/**
	* isChangeSceneセット
	*/
	void SetIsChangeScene(int isChangeScene) { this->isChangeScene_ = isChangeScene; }

private:

	//カメラ
	Camera *camera_;
	//プレイヤー
	Player* player_;
	XMFLOAT3 playerPos;

	//演出中か
	bool isPerformance_ = false;
	//演出中か前フレーム
	bool oldIsPerformance_ = false;
	//なんの演出か
	int performanceNum_ = TITLETOPLAY;
	//演出が始まったか(演出無→演出中)
	bool startPerformance_ = false;
	//シーンの切りかえ用変数
	int isChangeScene_ = false;

	//-----ゲームオーバ演出ー-----
	//どこまで降下させるか
	float downPosition_ = -12.0f;
	//降下スピード(Y)
	float downSpeed_ = 0.01f;
	//降下減速スピード(Z)
	float subSpeed_ = 0.001f;

	//-----汎用演出-----
	//スプライト
	std::vector<Sprite*> generalPurposeSprites_ = {};
	//スプライト数
	int generalPurposeSpritesSize_ = 2;

	//座標
	XMFLOAT2 generalPurposeSpritePosition1_ = {};
	XMFLOAT2 generalPurposeSpritePosition2_ = {};

	//イージング開始座標
	XMFLOAT2 generalPurposeEaseStartPosition1_ = {0 - (window_width / 2),window_height / 2};
	XMFLOAT2 generalPurposeEaseStartPosition2_ = {window_width + (window_width / 2),window_height / 2 };
	//イージング終了位置
	XMFLOAT2 generalPurposeEaseEndPosition1_ = { window_width + (window_width / 2),window_height / 2 };
	XMFLOAT2 generalPurposeEaseEndPosition2_ = { 0 - (window_width / 2),window_height / 2 };
	//イージング演出用データ
	Easing generalPurposeEaseing1_;
	Easing generalPurposeEaseing2_;
	//イージング演出時間
	float easeingTime_ = 6.0f;
};

