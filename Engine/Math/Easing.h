#pragma once
#include"Vector3.h"
#include<DirectXMath.h>

float Random(float num1, float num2);

DirectX::XMFLOAT2 EaseIn2D(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, float timer);
DirectX::XMFLOAT2 EaseOut2D(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, float timer);
DirectX::XMFLOAT3 EaseIn3D(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, float timer);
DirectX::XMFLOAT3 EaseOut3D(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, float timer);

class Easing
{
public:
	static long long nowTime;

	long long nowCount = 0;
	long long startCount = 0;
	long long endCount = 0;
	float maxTime = 0;
	float endTimer = 0;
	float timeRate = 0;


	void Update();

	void Start(float animationSecond);

	bool GetActive() { return (timeRate < 1.0f); }
};

