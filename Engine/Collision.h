#pragma once
#include <DirectXMath.h>
#include "wrl.h"

enum CollType {
	SQUARE,//矩形
	SPHERE//球
};

struct CollisionData {
	DirectX::XMFLOAT3 position_ = {};
	DirectX::XMFLOAT3 size = {};
	float radius = 0.0f;
};

class Collision
{
private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//矩形同士の当たり判定
	bool CheckSquareToSquare(CollisionData A, CollisionData B);
	//球同士の当たり判定
	bool CheckSphereToSphere(CollisionData A, CollisionData B);
};

