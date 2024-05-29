/**
* @file Collision.cpp
* @brief 当たり判定の処理
*/

#include "Collision.h"

bool Collision::CheckSquareToSquare(const CollisionData& A, const CollisionData& B)
{
	float positionAX1 = A.position.x - A.size.x / 2;
	float positionAX2 = A.position.x + A.size.x / 2;

	float positionAY1 = A.position.y - A.size.y / 2;
	float positionAY2 = A.position.y + A.size.y / 2;

	float positionAZ1 = A.position.z - A.size.z / 2;
	float positionAZ2 = A.position.z + A.size.z / 2;


	float positionBX1 = B.position.x - B.size.x / 2;
	float positionBX2 = B.position.x + B.size.x / 2;

	float positionBY1 = B.position.y - B.size.y / 2;
	float positionBY2 = B.position.y + B.size.y / 2;

	float positionBZ1 = B.position.z - B.size.z / 2;
	float positionBZ2 = B.position.z + B.size.z / 2;


	if (positionAX1 < positionBX2 && positionBX1 < positionAX2) {
		if (positionAY1 < positionBY2 && positionBY1 < positionAY2) {
			if (positionAZ1 < positionBZ2 && positionBZ1 < positionAZ2) {

				return true;

			}
		}
	}

	return false;
}

bool Collision::CheckSphereToSphere(const CollisionData& A, const CollisionData& B)
{
	float xDis = A.position.x - B.position.x;
	float yDis = A.position.y - B.position.y;
	float zDis = A.position.z - B.position.z;

	double distance = sqrt((xDis * xDis) + (yDis * yDis) + (zDis * zDis));

	if (distance <= A.radius + B.radius) {
		return true;
	}

	return false;
}

bool Collision::CheckSpriteTo2Dpos(const Sprite* sprite, const XMFLOAT2& pos)
{
	CollisionData A;

	A.position = { sprite->GetPosition().x,sprite->GetPosition().y,0 };
	A.size = { sprite->GetScale().x,sprite->GetScale().y,0 };

	float left = A.position.x - A.size.x / 2;
	float right = A.position.x + A.size.x / 2;

	float top = A.position.y - A.size.y / 2;
	float bottom = A.position.y + A.size.y / 2;

	if (left < pos.x && right > pos.x && top < pos.y && bottom > pos.y) {
		return true;
	}

	return false;

}
