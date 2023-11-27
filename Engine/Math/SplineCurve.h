/**
* @file SplineCurve.h
* @brief スプライン曲線
*/

#pragma once
#include"Vector3.h"
#include<DirectXMath.h>
#include<vector>

/**
* スプライン曲線処理
*
* @param[in] points 制御店の集合(vectorコンテナ)
* @param[in] startIndex 補間する区間の添え字
* @param[in] t 時間経過率
* 
* @return 座標
*/
Vector3 Spline3D(const std::vector<Vector3>& points, size_t startIndex, float t);

class SplineCurve
{
public:

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
	* スプラインスタート
	*
	* @param[in] animationSecond スプライン全体時間
	*/
	void Start(float animationSecond);
	/**
	* @return bool スプライン中か
	*/
	bool GetActive() { return (timeRate < 1.0f); }
};

