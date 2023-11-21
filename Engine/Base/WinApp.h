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

#define WINDOW_CLASS_NAME TEXT("SPEED")

class WinApp final
{
public:
	/**
	* シングルトンインスタンスを取得
	*/
	static WinApp* GetInstance();

	/**
	* コピーコンストラクタの無効
	*/
	WinApp(const WinApp& obj) = delete;

	/**
	* 代入演算子の無効
	*/
	WinApp& operator=(const WinApp& obj) = delete;

private:
	/**
	* コンストラクタ
	*/
	WinApp() = default;
	/**
	* デストラクタ
	*/
	~WinApp() = default;

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

