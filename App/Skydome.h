#pragma once
#include "DirectXCommon.h"
#include "FbxLoader.h"
#include "FbxObject3D.h"

class Skydome
{
public:
	void Initialize();
	void Update(float eyeZ);
	void Draw(ID3D12GraphicsCommandList* cmdList_);

	//端
	float GetEdge() { return scale_.z / 2; }

private:
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1000,1000,1000 };

	FbxModel* skydomeModel = nullptr;
	FbxObject3D* skydomeObject = nullptr;
};

