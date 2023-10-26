/**
* @file Camera.h
* @brief カメラの処理
*/

#pragma once
#include"Base/DirectXCommon.h"
#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>

#include"Input.h"
#include"Easing.h"

using namespace DirectX;

enum CameraMode {
	STRAIGHTMODE,//直線移動
	PLAYERFOLLOWMODE,//自機に追従
	TITLETOPLAYMODE,//タイトルからプレイ遷移
	BOSSINMODE,//ボス出現演出
	BOSSCLERAMODE,//ボス撃破演出
	BOSSGAMEOVERAMODE,//ボス逃亡演出
};

class Camera
{
public:
	static ID3D12Device* device_;

	/**
	* 静的変数初期化
	*
	* @param[in] dev デバイス
	*/
	static void StaticInitialize(ID3D12Device* dev);

	struct ConstBufferCamera {
		XMMATRIX view;
		XMMATRIX projection;
	};

	ConstBufferCamera* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

private:

	XMMATRIX matView_ = {};
	XMMATRIX matProjection_ = {};
	XMFLOAT3 eye_ = { 0, 1, 30 };
	XMFLOAT3 target_ = { 0, 0, 0 };
	XMFLOAT3 up_ = { 0, 1, 0 };;
	//描画最小距離
	float nearClip_ = 0.1f;
	//描画最大距離
	float farClip_ = 1000.0f;

public:

	/**
	* 初期化
	*
	*/
	void Initialize();
	/**
	* 更新
	*/
	void Update(XMFLOAT3 playerPos, XMFLOAT3 bossPos);
	/**
	* 行列更新
	*/
	void UpdateMatrix();

	/**
	* タイトルからプレイ遷移初期化
	*/
	void InitializeTitleToPlayMode();

	/**
	* 直線移動モード更新
	*/
	void UpdateStraightMode();
	/**
	* プレイヤー追従モード更新
	*/
	void UpdatePlayerFollowMode();
	/**
	* タイトルからプレイ遷移更新
	*/
	void UpdateTitleToPlayMode();

	/**
	* デバックモード
	*/
	void DebugMode();


	/**
	* @return XMMATRIX ビュープロジェクション取得
	*/
	XMMATRIX GetViewProjection()const { return matView_ * matProjection_; }
	/**
	* @return XMFLOAT3 eye_取得
	*/
	XMFLOAT3 GetEye() { return eye_; }
	/**
	* @return XMFLOAT3 target_取得
	*/
	XMFLOAT3 GetTarget() { return target_; }
	/**
	* @return int mode_取得
	*/
	int GetMode() { return mode_; }
	/**
	* @return bool isPerformance_取得
	*/
	bool GetIsPerformance() { return isPerformance_; }


	/**
	* eye_セット
	*/
	void SetEye(XMFLOAT3 eye) { this->eye_ = eye; }
	/**
	* target_セット
	*/
	void SetTarget(XMFLOAT3 target) { this->target_ = target; }
	/**
	* modeセット
	*/
	void SetMode(int mode);

private:

	//入力
	Input* input_ = nullptr;

	float angle_ = 0.1f; // カメラの回転角

	//プレイヤー座標
	XMFLOAT3 playerPos_ = {};
	//ボス座標
	XMFLOAT3 bossPos_ = {};

	//プレイヤーとカメラの距離
	float playerRange_ = 30.0f;

	//-----カメラモード-----
	int mode_ = STRAIGHTMODE;

	//カメラ直線移動モードスピード
	float straightModeSpeed_ = 1.0f;
	 
	//-----演出用-----
	//現在座標
	XMFLOAT3 startEye_ = {};
	XMFLOAT3 startTarget_ = {};

	//イージング終了位置
	XMFLOAT3 endEye_ = {};
	XMFLOAT3 endTarget_ = {};

	//現在のデータ保存用
	XMFLOAT3 holdEye_ = {};
	XMFLOAT3 holdTarget_ = {};

	//イージング演出用データ
	Easing easeing_;
	//演出時間
	float easeingTime_ = 3.0f;
	//演出中か
	bool isPerformance_ = false;
	//フェーズ
	int phase_ = 0;
};