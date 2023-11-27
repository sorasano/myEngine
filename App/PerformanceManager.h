/**
* @file PerformanceManager.h
* @brief 演出系
*/

#pragma once

#include "Scene.h"
#include "Camera.h"
#include "Sprite.h"
#include "Easing.h"
#include "SplineCurve.h"
#include "Player.h"
#include "Boss.h"

enum Performance {
	TITLETOPLAY,//タイトル→プレイシーン遷移
	INBOSS,//ボス戦導入
	CLEARBOSS,//ボス戦クリア
	GAMEOVERBOSS,//ボス戦ゲームオーバー
	RETURNTITLE,//タイトルに戻るとき
	OPENMENU,//メニューを開く
	CLOSEMENU,//メニューを閉じる
};

//クリア演出フェーズ
enum ClearPhase {
	CP_BOSSMOVE,//ボス墜落、カメラ追尾
	CP_PLAYERMOVE,//自機中心移動、カメラ旋回
	CP_END//演出終了
};

//ゲームオーバー演出フェーズ
enum GameoverPhase {
	GP_PLAYERMOVE,//自機墜落
	GP_CAMERAMOVE,//カメラ旋回
	GP_END//演出終了
};

class PerformanceManager
{
public:
	/**
	* 初期化
	*/
	void Initialize(Camera* camera,Player* player,Boss* boss);
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
	/**
	* メニューを開く演出
	*/
	void OpenMenuPerformance();
	/**
	* メニューを閉じる演出
	*/
	void CloseMenuPerformance();
	/**
	* メニューUI回転演出
	* 
	* @return 回転後座標
	*/
	void MenuUIRotPerformance(Sprite* menuUI);

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
	/**
	* oldSceneセット
	*/
	void SetIsOldScene(int isOldScene) { this->oldScene_ = isOldScene; }

private:

	//カメラ
	Camera *camera_;
	//プレイヤー
	Player* player_;
	//ボス
	Boss* boss_;

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

	//-----タイトルからゲームシーン遷移演出用-----
	//現在座標
	XMFLOAT3 titleToPlayEye_ = {};
	XMFLOAT3 titleToPlayTarget_ = {};

	//イージング開始座標
	XMFLOAT3 titleToPlayStartEye_ = {};
	XMFLOAT3 titleToPlayStartTarget_ = {};

	//イージング終了位置
	XMFLOAT3 titleToPlayEndEye_ = {};
	XMFLOAT3 titleToPlayEndTarget_ = {};

	//現在のデータ保存用
	XMFLOAT3 titleToPlayHoldEye_ = {};
	XMFLOAT3 titleToPlayHoldTarget_ = {};

	//イージング演出用データ
	Easing titleToPlayEaseing_;
	//演出時間
	float titleToPlayEaseingTime_ = 3.0f;
	//フェーズ
	int titleToPlayPhase_ = 0;

	//-----クリア演出ー-----
	//フェーズ
	int clearPhase_ = 0;
	//どこまで降下させるか(ボス)
	float clearDownPosition_ = -12.0f;
	//降下スピード(Y,ボス)
	float clearDownSpeed_ = 0.1f;
	//降下減速スピード(Z、ボス)
	float clearSubSpeed_ = 0.01f;
	//ボスのスピード
	float clearBossSpeed_ = 0.0f;

	//-----ゲームオーバ演出ー-----

	//ゲームオーバー演出用データS
	XMFLOAT3 gameoverPlayerPosition = {};
	float gameoverPlayerSpeed = {};

	XMFLOAT3 gameoverCameraEye = {};
	XMFLOAT3 gameoverCameraTarget = {};

	//フェーズ
	int gameoverPhase_ = 0;
	//どこまで降下させるか(プレイヤー)
	float gemeoverDownPosition_ = -11.0f;
	//どれだけ減速させていくか
	float gameoverPlayerDownSpeed = 0.01f;

	//イージング用変数
	Easing gemeoverDownEaseing_;
	XMFLOAT3 gemeoverDownEaseStartPosition_ = {};
	XMFLOAT3 gemeoverDownEaseEndPosition_ = {};

	//イージング全体時間
	float gemeoverEaseingTime_ = 3.0f;

	//スプライン用変数
	//cameraEye
	SplineCurve gemeoverEyeSpline_;
	std::vector<Vector3> gemeoverEyeSplinePoints_ = {};
	Vector3 gemeoverSplineEye_ = {};
	//スプライン1ポイント時間
	float gemeoverEyeSplineTime_ = 1.0f;
	//今のスタートポイント
	size_t gemeoverEyeStartIndex_ = 0;

	//cameraTarget
	SplineCurve gemeoverTargetSpline_;
	std::vector<Vector3> gemeoverTargetSplinePoints_ = {};
	Vector3 gemeoverSplineTarget_ = {};
	//スプライン1ポイント時間
	float gemeoverTargetSplineTime_ = gemeoverEyeSplineTime_ * 3;
	//今のスタートポイント
	size_t gemeoverTargetStartIndex_ = 0;

	//-----汎用演出-----
	//スプライト
	std::vector<Sprite*> generalPurposeSprites_ = {};
	//スプライト数
	int generalPurposeSpritesSize_ = 2;

	//座標
	XMFLOAT2 generalPurposeSpritePosition1_ = {};
	XMFLOAT2 generalPurposeSpritePosition2_ = {};

	//イージング開始座標
	XMFLOAT2 generalPurposeEaseStartPosition1_ = { 0 - (window_width / 2),window_height / 2 };
	XMFLOAT2 generalPurposeEaseStartPosition2_ = { window_width + (window_width / 2),window_height / 2 };
	//イージング終了位置
	XMFLOAT2 generalPurposeEaseEndPosition1_ = { window_width + (window_width / 2) + 100,window_height / 2 };
	XMFLOAT2 generalPurposeEaseEndPosition2_ = { 0 - (window_width / 2) - 100,window_height / 2 };
	//イージング演出用データ
	Easing generalPurposeEaseing1_;
	Easing generalPurposeEaseing2_;
	//イージング演出時間
	float generalPurposeEaseingTime_ = 3.0f;

	//-----メニュー-----
	//メニュー開いた時のシーン
	int oldScene_;

	//メニューUI回転
	float menuUIRot = 0.0f;
	//メニューUI回転度数
	float menuUIAddRot = 1.0f;

};

