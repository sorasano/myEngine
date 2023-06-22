#include "Collision.h"

using namespace DirectX;

bool Collision::CheckSphere2Plane(const SphereCol& sphere, const PlaneCol& plane, DirectX::XMVECTOR* inter)
{
	//���W�n�̌��_���狅�̒��S���W�ւ̋���
	XMVECTOR distV = XMVector3Dot(sphere.center,plane.normal);

	//���ʂ̌��_���������Z���邱�Ƃ�,���ʂƋ��̒��S�Ƃ̋������o��
	float dist = distV.m128_f32[0] - plane.distance;

	//�����̐�Βl�����a���傫����Γ������Ă��Ȃ�
	if (fabsf(dist) > sphere.radius) return false;

	//�^�������_���v�Z

	if (inter) {

		//���ʏ�̍ŋߐړ_���A�^�������_�Ƃ���
		*inter = -dist * plane.normal + sphere.center;

	}

	return true;
}
