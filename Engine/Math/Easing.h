/**
* @file Easing.h
* @brief イージング処理
*/

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

	/**
	* 更新
	*/
	void Update();
	/**
	* イージングスタート
	*
	* @param[in] animationSecond イージング全体時間
	*/
	void Start(float animationSecond);
	/**
	* @return bool イーイング中か
	*/
	bool GetActive() { return (timeRate < 1.0f); }
};

