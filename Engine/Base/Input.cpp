/**
* @file Input.cpp
* @brief 入力処理
*/

#include "Input.h"

//シングルトンインスタンスを取得
Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize() {
	
	HRESULT result;

	//借りてきたWinAppのインスタンスを記録
	this->winApp_ = WinApp::GetInstance();

	//キーボード処理

	// DirectInputの初期化

	result = DirectInput8Create(
		winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

}

void Input::Update() {

	// キーボード情報の取得開始
	keyboard->Acquire();

	//前回のキー入力を保存
	memcpy(oldkey, key, sizeof(key));

	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);

	//パッドの接続確認
	oldPadState = padState;
	DWORD flag;
	flag = XInputGetState(0, &padState);
}

//bool Input::PushKey(BYTE keyNumber)
//{
//	//指定のキーを押していればtrueを返す
//	if (key[keyNumber]) {
//		return true;
//	}
//
//	//そうでなけえればfalseを返す
//	return false;
//}
//
//bool Input::TriggerKey(BYTE keyNumber)
//{
//	//指定のキーを押していればtrueを返す
//	if (key[keyNumber] && keyPre[keyNumber] == false) {
//		return true;
//	}
//
//	return false;
//}

bool Input::IsKeyTrigger(BYTE key_)
{
	return (key[key_] && !oldkey[key_]);
}

bool Input::IsKeyPress(BYTE key_)
{
	return key[key_];
}

bool Input::IsKeyRelease(BYTE key_)
{
	return (!key[key_] && oldkey[key_]);
}

bool Input::IsPadTrigger(WORD Button)
{
	return (padState.Gamepad.wButtons & Button) && ((oldPadState.Gamepad.wButtons & Button) != Button);
}

bool Input::IsPadPress(WORD Button)
{
	return padState.Gamepad.wButtons & Button;
}

bool Input::IsPadRelease(WORD Button)
{
	return (oldPadState.Gamepad.wButtons & Button) && ((padState.Gamepad.wButtons & Button) != Button);
}

bool Input::IsDownLStickLeft(int deadZone)
{
	return padState.Gamepad.sThumbLX < -deadZone;
}

bool Input::IsTriggerLStickLeft(int deadZone)
{
	return (padState.Gamepad.sThumbLX < -deadZone) && (oldPadState.Gamepad.sThumbLX >= -deadZone);
}

bool Input::IsDownLStickRight(int deadZone)
{
	return padState.Gamepad.sThumbLX > deadZone;
}

bool Input::IsTriggerLStickRight(int deadZone)
{
	return (padState.Gamepad.sThumbLX > deadZone) && (oldPadState.Gamepad.sThumbLX <= deadZone);
}

bool Input::IsDownLStickUp(int deadZone)
{
	return padState.Gamepad.sThumbLY > deadZone;
}

bool Input::IsTriggerLStickUp(int deadZone)
{
	return (padState.Gamepad.sThumbLY > deadZone) && (oldPadState.Gamepad.sThumbLY <= deadZone);
}

bool Input::IsDownLStickDown(int deadZone)
{
	return padState.Gamepad.sThumbLY < -deadZone;
}

bool Input::IsTriggerLStickDown(int deadZone)
{
	return (padState.Gamepad.sThumbLY < -deadZone) && (oldPadState.Gamepad.sThumbLY >= -deadZone);
}

bool Input::IsDownRStickLeft(int deadZone)
{
	return padState.Gamepad.sThumbRX < -deadZone;
}

bool Input::IsTriggerRStickLeft(int deadZone)
{
	return (padState.Gamepad.sThumbRX < -deadZone) && (oldPadState.Gamepad.sThumbRX >= -deadZone);
}

bool Input::IsDownRStickRight(int deadZone)
{
	return padState.Gamepad.sThumbRX > deadZone;
}

bool Input::IsTriggerRStickRight(int deadZone)
{
	return (padState.Gamepad.sThumbRX > deadZone) && (oldPadState.Gamepad.sThumbRX <= deadZone);
}

bool Input::IsDownRStickUp(int deadZone)
{
	return padState.Gamepad.sThumbRY > deadZone;
}

bool Input::IsTriggerRStickUp(int deadZone)
{
	return (padState.Gamepad.sThumbRY > deadZone) && (oldPadState.Gamepad.sThumbRY <= deadZone);
}

bool Input::IsDownRStickDown(int deadZone)
{
	return padState.Gamepad.sThumbRY < -deadZone;
}

bool Input::IsTriggerRStickDown(int deadZone)
{
	return (padState.Gamepad.sThumbRY < -deadZone) && (oldPadState.Gamepad.sThumbRY >= -deadZone);
}
