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
	DEBUGMODE,//デバックモード
};

class Camera
{
public:
	static ID3D12Device* device;

	static void StaticInitialize(ID3D12Device* dev);

	struct ConstBufferCamera {
		XMMATRIX view;
		XMMATRIX projection;
	};

	ConstBufferCamera* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	XMMATRIX matView = {};
	XMMATRIX matProjection = {};
	XMFLOAT3 eye = { 0, 1, 30 };
	XMFLOAT3 target = { 0, 0, 0 };
	XMFLOAT3 up = { 0, 1, 0 };;

public:


	void Initialize(Input* input);

	void UpdateMatrix();

	void Update(XMFLOAT3 playerPos, XMFLOAT3 bossPos);

	//直線移動モード
	void UpdateStraightMode();
	//プレイヤー追従モード
	void UpdatePlayerFollowMode();

	//タイトルからプレイ遷移更新
	void UpdateTitleToPlayMode();
	//タイトルからプレイ遷移初期化
	void InitializeTitleToPlayMode();

	//デバックモード
	void DebugMode();

	XMMATRIX GetViewProjection()const { return matView * matProjection; }
	XMFLOAT3 GetEye() { return eye; }
	XMFLOAT3 GetTarget() { return target; }
	int GetMode() { return mode; }
	float GetRangeMaxZ() { return rangeMaxZ; }
	bool GetIsPerformance() { return isPerformance; }

	void SetEye(XMFLOAT3 eye) { this->eye = eye; }
	void SetTarget(XMFLOAT3 target) { this->target = target; }
	void SetMode(int mode);

private:

	//入力
	Input* input_ = nullptr;

	float angle = 0.1f; // カメラの回転角

	//プレイヤー座標
	XMFLOAT3 playerPos_ = {};
	//ボス座標
	XMFLOAT3 bossPos_ = {};

	//描画最大距離
	float rangeMaxZ = 500.0f;
	//プレイヤーとカメラの距離
	float playerRange = 30.0f;

	//-----カメラモード-----
	int mode = STRAIGHTMODE;

	//カメラ直線移動モードスピード
	float straightModeSpeed = 1.0f;

	//-----演出用-----
	//現在座標
	XMFLOAT3 startEye = {};
	XMFLOAT3 startTarget = {};

	//イージング終了位置
	XMFLOAT3 endEye = {};
	XMFLOAT3 endTarget = {};

	//現在のデータ保存用
	XMFLOAT3 holdEye = {};
	XMFLOAT3 holdTarget = {};

	//イージング演出用データ
	Easing easeing;
	//演出時間
	float easeingTime = 3.0f;
	//演出中か
	bool isPerformance = false;
	//フェーズ
	int phase = 0;
};