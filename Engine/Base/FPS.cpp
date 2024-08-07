/**
* @file FPS.cpp
* @brief FPSの設定
*/

#include "FPS.h"

void FPS::FpsControlBegin() {
	//周波数取得
	QueryPerformanceFrequency(&cpuClock);
	//計測開始時間の初期化
	QueryPerformanceCounter(&timeStart);
}

void FPS::FpsControlEnd() {
	//今の時間を取得
	QueryPerformanceCounter(&timeEnd);
	//経過時間
	float elapsedFrame = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) /
		static_cast<float>(cpuClock.QuadPart);
	//余裕があるときは待つ
	if (elapsedFrame < frameTime_) {
		// sleep時間
		DWORD sleepTime = static_cast<DWORD>((frameTime_ - elapsedFrame) * 1000.0f);
		timeBeginPeriod(1);
		//寝る
		Sleep(sleepTime);
		timeEndPeriod(1);
	}

	fps = 1 / elapsedFrame;
}

void FPS::SetFrameRate(float fps_) { frameTime_ = 1.0f / fps_; }

float FPS::GetFrameRate() { return fps; }