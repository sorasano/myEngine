#pragma once

#include "CollisionPrimitive.h"

class Collision
{
public:

	//���ƕ��ʂ̓����蔻��
	static bool CheckSphere2Plane(const SphereCol& sphere,const PlaneCol&plane, DirectX::XMVECTOR*inter = nullptr);

};

