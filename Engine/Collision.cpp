#include "Collision.h"

bool Collision::CheckSquareToSquare(CollisionData A, CollisionData B)
{
	float positionAX1 = A.position_.x - A.size.x / 2;
	float positionAX2 = A.position_.x + A.size.x / 2;

	float positionAY1 = A.position_.y - A.size.y / 2;
	float positionAY2 = A.position_.y + A.size.y / 2;

	float positionAZ1 = A.position_.z - A.size.z / 2;
	float positionAZ2 = A.position_.z + A.size.z / 2;


	float positionBX1 = B.position_.x - B.size.x / 2;
	float positionBX2 = B.position_.x + B.size.x / 2;

	float positionBY1 = B.position_.y - B.size.y / 2;
	float positionBY2 = B.position_.y + B.size.y / 2;

	float positionBZ1 = B.position_.z - B.size.z / 2;
	float positionBZ2 = B.position_.z + B.size.z / 2;


	if (positionAX1 < positionBX2 && positionBX1 < positionAX2) {
		if (positionAY1 < positionBY2 && positionBY1 < positionAY2) {
			if (positionAZ1 < positionBZ2 && positionBZ1 < positionAZ2) {

				return true;

			}
		}
	}

	return false;
}

bool Collision::CheckSphereToSphere(CollisionData A, CollisionData B)
{
	float xDis = A.position_.x - B.position_.x;
	float yDis = A.position_.y - B.position_.y;
	float zDis = A.position_.z - B.position_.z;

	double distance = sqrt((xDis * xDis) + (yDis * yDis) + (zDis * zDis));

	if (distance <= A.radius + B.radius) {
		return true;
	}

	return false;
}
