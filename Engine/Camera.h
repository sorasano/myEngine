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
	PLAYERTURNINGMODE,//自機を旋回
	PERFORMANCEMODE,//演出モード(パファーマンスマネージャーで操作)
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
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};

	ConstBufferCamera* constMap = nullptr;
	ComPtr<ID3D12Resource> constBuff_;

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
	void Update(XMFLOAT3 playerPos);
	/**
	* 行列更新
	*/
	void UpdateMatrix();

	/**
	* 直線移動モード更新
	*/
	void UpdateStraightMode();
	/**
	* プレイヤー追従モード更新
	*/
	void UpdatePlayerFollowMode();
	/**
	* プレイヤー旋回モード更新
	*/
	void UpdatePlayerTurningMode();

	/**
	* デバックモード
	*/
	void DebugMode();
	/**
	* リセット
	*/
	void Reset();


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
	* @return float farClip_取得
	*/
	float GetFarClip() { return farClip_; }
	/**
	* @return float playerRange_取得
	*/
	float GetplayerRange() { return playerRange_; }

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
	void SetMode(int mode) { this->mode_ = mode; }

private:

	//入力
	Input* input_ = nullptr;

	//プレイヤー座標
	XMFLOAT3 playerPos_ = {};

	//プレイヤーとカメラの距離
	float playerRange_ = 30.0f;

	//-----カメラモード-----
	int mode_ = STRAIGHTMODE;

	//カメラ直線移動モードスピード
	float straightModeSpeed_ = 1.0f;
	 
	//カメラ初期座標
	XMFLOAT3 initEye_ = { 0.0f,0.0f,0.0f };
	XMFLOAT3 initTarget_ = { 0.0f,0.0f,1.0f };

	//カメラ円運動用角度
	float angle_ = 0.1f; // カメラの回転角
	//何度づつ足すか
	float addAngle_ = 1.0f;
	//回転半径の長さ
	float turnLength_ = 10.0f;

};