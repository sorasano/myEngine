#pragma once
#include <DirectXMath.h>
#include "wrl.h"

enum CollType {
	SQUARE,//��`
	SPHERE//��
};

struct CollisionData {
	DirectX::XMFLOAT3 position_ = {};
	DirectX::XMFLOAT3 size = {};
	float radius = 0.0f;
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
	//��`���m�̓����蔻��
	bool CheckSquareToSquare(CollisionData A, CollisionData B);
	//�����m�̓����蔻��
	bool CheckSphereToSphere(CollisionData A, CollisionData B);
};

