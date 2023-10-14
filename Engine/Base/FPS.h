/**
* @file FPS.h
* @brief FPSの設定
*/

#pragma once
#include <fstream>
#include <sstream>
#include <windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

class FPS {
private:
	// CPU周波数
	LARGE_INTEGER cpuClock{};
	//計測開始時間
	LARGE_INTEGER timeStart{};
	//計測終了時間
	LARGE_INTEGER timeEnd{};
	//固定する時間
	float frameTime_ = 1 / 60.0f;
	// FPS値
	float fps;

public:
	/**
	* FPS制御初期化
	*/
	void FpsControlBegin();

	/**
	* FPS制御
	*/
	void FpsControlEnd();

	/**
	* フレームレートを設定
	*
	* @param[in] fps_ フレームレート
	*/
	void SetFrameRate(float fps_);

	/**
	*  @return float フレームレート取得
	*/
	float GetFrameRate();
};

