#pragma once

#include "Model.h"
#include "Object3D.h"
#include "Base/DirectXCommon.h"

struct PlaneCol {
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float distance = 0.0f;
};

class Plane
{
public:
	//シングルトンインスタンス
	Plane* GetInstance();
	Plane();
	~Plane();
	void Initialize(DirectXCommon* dx, Model* model);
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	void Draw();
	//ゲッター　セッター　
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void setPosition(XMFLOAT3 pos);
	void setRotation(XMFLOAT3 rot);
	void setScale(XMFLOAT3 sca);

	PlaneCol planeCol;

private:
	DirectXCommon* dx_ = nullptr;
	Model* model_ = nullptr;
	std::unique_ptr<Object3D> object3d_;
	//アフィン変換情報
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
};

