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
	BOSSAPPEAREARANCEMODE,//ボス出現演出
	BOSSDESTROYMODE,//ボス撃破演出
	CHANGEMODE,//モードの切り替え
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

	//モード変更モード
	void UpdateChangeMode();
	//モード変更モードチェック
	void CheckChangeMode();

	//デバックモード
	void DebugMode();

	XMMATRIX GetViewProjection()const { return matView * matProjection; }
	XMFLOAT3 GetEye() { return eye; }
	XMFLOAT3 GetTarget() { return target; }
	int GetMode() { return mode; }

	void SetEye(XMFLOAT3 eye) { this->eye = eye; }
	void SetTarget(XMFLOAT3 target) { this->target = target; }
	void SetMode(int mode) { this->mode = mode; }

private:

	//入力
	Input* input_ = nullptr;

	float angle = 0.1f; // カメラの回転角

	//プレイヤー座標
	XMFLOAT3 playerPos_ = {};
	//ボス座標
	XMFLOAT3 bossPos_ = {};

	//-----カメラモード-----
	int mode = STRAIGHTMODE;
	//前フレームのモード
	int oldMode = STRAIGHTMODE;
	//現在のモード保持用
	int holdMode = STRAIGHTMODE;


	//カメラ直線移動モードスピード
	float straightModeSpeed = 1.0f;

	//-----カメラモード変更時演出用-----
	//現在座標
	XMFLOAT3 nowEye = {};
	XMFLOAT3 nowTarget = {};

	//次のモード開始位置
	XMFLOAT3 nextEye = {};
	XMFLOAT3 nextTarget = {};

	//現在のデータ保存用
	XMFLOAT3 holdEye = {};
	XMFLOAT3 holdTarget = {};

	//イージング演出用データ
	Easing easeing;
	//演出時間
	float easeingTime = 1.0f;
};