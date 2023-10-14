/**
* @file WinApp.h
* @brief WinApp
*/

#pragma once
#include <wrl.h>

using namespace Microsoft::WRL;

// ウィンドウサイズ
static const int window_width = 1280; // 横幅
static const int window_height = 720; // 縦幅

class WinApp
{
public:
	/**
	* ウィンドウプロシージャ
	*
	* @param[in] hwnd ウィンドウハンドル
	* @param[in] msg メッセージ コード
	* @param[in] wparam メッセージに関連する追加のデータ
	* @param[in] lparam メッセージに関連する追加のデータ
	* @return LRESULT 標準のメッセージ処理
	*/
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	/**
	* 初期化
	*/
	void Initialize();

	/**
	* 終了
	*/
	void Finalize();

	/**
	* メッセージの処理
	*/
	bool processMessage();

	/**
	* @return HWND hwnd取得
	*/
	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHInstance() const { return w.hInstance; }
public:
	//ウィンドウハンドル
	HWND hwnd = nullptr;

	// ウィンドウクラスの設定
	WNDCLASSEX w{};
};

