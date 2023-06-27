#pragma once
#include"Base/DirectXCommon.h"
#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>
//#include "Vector3.h"
#include"Base/Input.h"

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
	XMFLOAT3 eye = {};
	XMFLOAT3 target = {};
	XMFLOAT3 up = {} ;

public:


	void Initialize(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up, Input* input);

	void UpdateMatrix();

	void Update();

	//eyeの更新
	void UpdateEye();

	//targetの更新
	void UpdateTarget();

	XMMATRIX GetViewProjection()const { return matView * matProjection; }
	XMFLOAT3 GetEye() { return eye; }

	void SetEye(XMFLOAT3 eye) { this->eye = eye; }
	void SetTarget(XMFLOAT3 target) { this->target = target; }

private:

	Input* input = nullptr;

	float angle = 0.1f; // カメラの回転角

};