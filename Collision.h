#pragma once

#include "CollisionPrimitive.h"

class Collision
{
public:

	//球と平面の当たり判定
	static bool CheckSphere2Plane(const SphereCol& sphere,const PlaneCol&plane, DirectX::XMVECTOR*inter = nullptr);

};

