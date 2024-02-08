/**
* @file Input.h
* @brief 入力処理
*/

#pragma once

#include <Windows.h>
#include "WinApp.h"
#include <cassert>

#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include<wrl.h>
#include<Xinput.h>
#pragma comment(lib,"xinput.lib")

#include <DirectXMath.h>

//////////////////////////////////////////
/////////////PADのボタン一覧////////////////
/////////////////////////////////////////
//XINPUT_GAMEPAD_DPAD_UP				スティック上
//XINPUT_GAMEPAD_DPAD_DOWN				スティック下
//XINPUT_GAMEPAD_DPAD_LEFT				スティック左
//XINPUT_GAMEPAD_DPAD_RIGHT				スティック右
//XINPUT_GAMEPAD_START					スタートボタン
//XINPUT_GAMEPAD_BACK					セレクトボタン
//XINPUT_GAMEPAD_LEFT_THUMB				左スティック押し込み
//XINPUT_GAMEPAD_RIGHT_THUMB			右スティック押し込み
//XINPUT_GAMEPAD_LEFT_SHOULDER			Lボタン(LBは別)
//XINPUT_GAMEPAD_RIGHT_SHOULDER			Rボタン(RBは別)
//XINPUT_GAMEPAD_A						Aボタン
//XINPUT_GAMEPAD_B						Bボタン
//XINPUT_GAMEPAD_X						Xボタン
//XINPUT_GAMEPAD_Y						Yボタン

using XMFLOAT2 = DirectX::XMFLOAT2;

enum MouseButton {
	LEFT_CLICK,
	RIGHT_CLICK,
	MID_CLICK
};

namespace MyEngine {
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
		Input() = default;
		/**
		* デストラクタ
		*/
		~Input() = default;

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
		* ウィンドウ外制限
		*/
		void WindowLock();

		/**
		* 	ウィンドウ外制限解除
		*/
		void WindowUnLock();

		/**
		* 	ウィンドウ内かチェック
		*
		* @return false = ウィンドウ外 true ウィンドウ内
		*/
		bool CheckInWindow();

		/**
		* @return bool isLockInWindow取得
		*/
		bool GetIsLockInWindow() { return isLockInWindow; }


		//入力情報

		//キーボード押下情報
		/**
		* キーの入力瞬間チェック
		*/
		bool IsKeyTrigger(BYTE key_);
		/**
		* キーの押込みをチェック
		*/
		bool IsKeyPress(BYTE key_);
		/**
		* キーの離しをチェック
		*/
		bool IsKeyRelease(BYTE key_);

		//マウス

		/**
		* 座標取得
		*/
		XMFLOAT2 GetMousePosition();

		//マウス押下情報
		/**
		* キーの入力瞬間チェック
		*/
		bool IsMouseTrigger(MouseButton buttonType);
		/**
		* キーの押込みをチェック
		*/
		bool IsMousePress(MouseButton keybuttonType_);
		/**
		* キーの離しをチェック
		*/
		bool IsMouseRelease(MouseButton buttonType);


		//パッド押下情報(ボタン)
		/**
		* ボタンの入力瞬間をチェック
		*/
		bool IsPadTrigger(WORD Button);
		/**
		* ボタンの押込みをチェック
		*/
		bool IsPadPress(WORD Button);
		/**
		* ボタンの離しをチェック
		*/
		bool IsPadRelease(WORD Button);


		//パッド押下情報(左右スティック)
		/**
		* 左スティック
		*/
		bool IsDownLStickLeft(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsTriggerLStickLeft(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsDownLStickRight(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsTriggerLStickRight(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsDownLStickUp(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsTriggerLStickUp(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsDownLStickDown(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		bool IsTriggerLStickDown(int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		/**
		* 右スティック
		*/
		bool IsDownRStickLeft(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsTriggerRStickLeft(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsDownRStickRight(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsTriggerRStickRight(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsDownRStickUp(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsTriggerRStickUp(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsDownRStickDown(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		bool IsTriggerRStickDown(int deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

	private:

		ComPtr<IDirectInputDevice8> keyboard;
		ComPtr<IDirectInput8> directInput;

		ComPtr<IDirectInputDevice8> mouse;

		//キーボード
		BYTE key[256] = {};
		BYTE oldkey[256] = {};

		//パッド
		XINPUT_STATE padState;
		XINPUT_STATE oldPadState;

		//マウス
		DIMOUSESTATE mouseState;
		DIMOUSESTATE oldMouseState;

		//マウスウィンドウズ画面内に固定されているか
		bool isLockInWindow = true;

		//WindowsAPI
		WinApp* winApp_ = nullptr;
	};
}
