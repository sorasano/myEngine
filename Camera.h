#pragma once
#include"Base/DirectXCommon.h"
#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>

#include"Input.h"

using namespace DirectX;

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

	void Update(XMFLOAT3 playerPos);

	//eyeの更新
	void UpdateEye();

	//targetの更新
	void UpdateTarget();

	void DebugMode();

	XMMATRIX GetViewProjection()const { return matView * matProjection; }
	XMFLOAT3 GetEye() { return eye; }
	XMFLOAT3 GetTarget() { return target; }

	void SetEye(XMFLOAT3 eye) { this->eye = eye; }
	void SetTarget(XMFLOAT3 target) { this->target = target; }

private:

	Input* input_ = nullptr;

	float angle = 0.1f; // カメラの回転角

	XMFLOAT3 playerPos_ = {};

};