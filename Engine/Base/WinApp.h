#pragma once
#include <wrl.h>

using namespace Microsoft::WRL;

// ウィンドウサイズ
static const int window_width = 1280; // 横幅
static const int window_height = 720; // 縦幅

class WinApp
{
public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


public:

	void Initialize();

	//終了
	void Finalize();

	//メッセージの処理
	bool processMessage();

	//getter
	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHInstance() const { return w.hInstance; }
public:
	//ウィンドウハンドル
	HWND hwnd = nullptr;

	// ウィンドウクラスの設定
	WNDCLASSEX w{};

	static const int winW = 1280;
	static const int winH = 720;
};

