/**
* @file Input.h
* @brief 入力処理
*/

#include <cassert>

#include <wrl.h>

using namespace Microsoft::WRL;

#pragma once
#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <Windows.h>
#include "WinApp.h"

class Input final
{
private:

	/**
	* namespace省略
	*/
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;

public:
	/**
	* シングルトンインスタンスを取得
	*/
	static Input* GetInstance();

	/**
	* コピーコンストラクタの無効
	*/
	Input(const Input& obj) = delete;

	/**
	* 代入演算子の無効
	*/
	Input& operator=(const Input& obj) = delete;

private:

	/**
	* コンストラクタ
	*/
	Input() = default;;
	/**
	* デストラクタ
	*/
	~Input() = default;;

public:

	/**
	* 初期化
	*/
	void Initialize();
	/**
	* 更新
	*/
	void Update();
	/**
	* キーの押下をチェック
	*/
	bool PushKey(BYTE keyNumber);
	/**
	* キーのトリガーをチェック
	*/
	bool TriggerKey(BYTE keyNumber);

private:

	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInput8> directInput;

	BYTE key[256] = {};
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};

