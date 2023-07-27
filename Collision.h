#pragma once
#include <DirectXMath.h>
#include "wrl.h"

struct CollisionData {
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 size = {};
};

class Collision
{
private:	//�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	bool CheckCollisionSquare(CollisionData A, CollisionData B);
};

