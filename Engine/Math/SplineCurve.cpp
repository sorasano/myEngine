/**
* @file SplineCurve.cpp
* @brief スプライン曲線
*/

#include "SplineCurve.h"
#include<random>
#include<time.h>
#include<cmath>
using namespace std;

Vector3 Spline3D(const std::vector<Vector3>& points, size_t startIndex, float t)
{

	//補間すべき点の数
	size_t n = points.size() - 2;

	if (startIndex > n)return points[n];
	if (startIndex < 1)return points[1];

	Vector3 p0 = points[startIndex - 1];
	Vector3 p1 = points[startIndex];
	Vector3 p2 = points[startIndex + 1];
	Vector3 p3 = points[startIndex + 2];

	Vector3 position = 0.5 * (2 * p1 + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * (t * t) + (-p0 + 3 * p1 - 3 * p2 + p3) * (t * t * t));

	return position;

}

void SplineCurve::Update()
{

	//現在時刻を取得
	nowCount = clock();
	//取得されたスプライン開始時間で終了時間が決まる
	endCount = nowCount - startCount;
	//タイマーの進み具合をfloat型に変換
	endTimer = static_cast<float>(endCount) / 1'000.0f;
	//(タイマーの進行度/最大時間)でスプラインの進み具合を計算
	timeRate = min(endTimer / maxTime, 1.0f);
	//timeRate = endTimer / maxTime;

}

void SplineCurve::Start(float animationSecond)
{
	startCount = clock();
	maxTime = animationSecond;

	timeRate = 0.0f;

}
