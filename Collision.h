#pragma once

#include "CollisionPrimitive.h"

class Collision
{
public:

	//‹…‚Æ•½–Ê‚Ì“–‚½‚è”»’è
	static bool CheckSphere2Plane(const SphereCol& sphere,const PlaneCol&plane, DirectX::XMVECTOR*inter = nullptr);

};

