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

	//// 最新のキーボード情報だったものは1フレーム前のキーボード情報として保存
	//for (int i = 0; i < 256; i++) {
	//	oldkey[i] = key[i];
	//}

	//前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber)
{
	//指定のキーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}

	//そうでなけえればfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	//指定のキーを押していればtrueを返す
	if (key[keyNumber] && keyPre[keyNumber] == false) {
		return true;
	}

	return false;
}
