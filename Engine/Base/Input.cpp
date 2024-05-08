/**
* @file Input.cpp
* @brief 入力処理
*/

#include "Input.h"

//シングルトンインスタンスを取得
MyEngine::Input* MyEngine::Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void MyEngine::Input::Initialize() {
	
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


	// マウスデバイスの生成

	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセットmouse
	result = mouse->SetDataFormat(& c_dfDIMouse); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = mouse->SetCooperativeLevel(
		winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//マウスをウィンドウ外に出ないように
	//WindowLock();
}

void MyEngine::Input::Update() {

	// キーボード情報の取得開始
	keyboard->Acquire();

	//前回のキー入力を保存
	memcpy(oldkey, key, sizeof(key));

	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);

	//マウスの状態を更新

	// 更新前に最新マウス情報を保存する
	oldMouseState = mouseState;

	// 最新のマウスの状態を更新
	HRESULT	hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	if (FAILED(hr))
	{
		mouse->Acquire();
		hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	}

	//パッドの接続確認
	oldPadState = padState;
	DWORD flag;
	flag = XInputGetState(0, &padState);

}

void MyEngine::Input::WindowLock()
{
	//マウスをウィンドウ外に出ないように

	// Get the window client area.
	RECT rc;
	GetClientRect(winApp_->GetHwnd(), &rc);

	// Convert the client area to screen coordinates.
	POINT pt = { rc.left, rc.top };
	POINT pt2 = { rc.right, rc.bottom };
	ClientToScreen(winApp_->GetHwnd(), &pt);
	ClientToScreen(winApp_->GetHwnd(), &pt2);
	SetRect(&rc, pt.x, pt.y, pt2.x, pt2.y);

	// Confine the cursor.
	ClipCursor(&rc);

	isLockInWindow = true;
}

void MyEngine::Input::WindowUnLock()
{
	ClipCursor(NULL);

	isLockInWindow = false;
}

bool MyEngine::Input::CheckInWindow()
{
	//ウィンドウ内外判定
	XMFLOAT2 pos = GetMousePosition();

	if (pos.x >= 0 && pos.x <= window_width) {
		if (pos.y >= 0 && pos.y <= window_height) {
			return true;
		}
	}

	return false;
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

bool MyEngine::Input::IsKeyTrigger(BYTE key_)
{
	return (key[key_] && !oldkey[key_]);
}

bool MyEngine::Input::IsKeyPress(BYTE key_)
{
	return key[key_];
}

bool MyEngine::Input::IsKeyRelease(BYTE key_)
{
	return (!key[key_] && oldkey[key_]);
}

XMFLOAT2 MyEngine::Input::GetMousePosition()
{

	//マウス座標取得
	POINT p;
	GetCursorPos(&p);

	//スクリーン座標をクライアント座標に変換
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd,&p);

	return XMFLOAT2((float)p.x,(float)p.y);
}

bool MyEngine::Input::IsMouseTrigger(MouseButton buttonType)
{
	//ウィンドウ外の場合取得しない
	if (!CheckInWindow()) {
		return false;
	}

	return (!(oldMouseState.rgbButtons[LEFT_CLICK] != buttonType) &&
		mouseState.rgbButtons[LEFT_CLICK] != buttonType);
}

bool MyEngine::Input::IsMousePress(MouseButton buttonType)
{
	//ウィンドウ外の場合取得しない
	if (!CheckInWindow()) {
		return false;
	}

	return mouseState.rgbButtons[LEFT_CLICK] != buttonType;
}

bool MyEngine::Input::IsMouseRelease(MouseButton buttonType)
{
	//ウィンドウ外の場合取得しない
	if (!CheckInWindow()) {
		return false;
	}

	return (oldMouseState.rgbButtons[LEFT_CLICK] != buttonType &&
		!(mouseState.rgbButtons[LEFT_CLICK] != buttonType));
}


bool MyEngine::Input::IsPadTrigger(WORD Button)
{
	return (padState.Gamepad.wButtons & Button) && ((oldPadState.Gamepad.wButtons & Button) != Button);
}

bool MyEngine::Input::IsPadPress(WORD Button)
{
	return padState.Gamepad.wButtons == Button;
}

bool MyEngine::Input::IsPadRelease(WORD Button)
{
	return (oldPadState.Gamepad.wButtons & Button) && ((padState.Gamepad.wButtons & Button) != Button);
}

bool MyEngine::Input::IsDownLStickLeft(int deadZone)
{
	return padState.Gamepad.sThumbLX < -deadZone;
}

bool MyEngine::Input::IsTriggerLStickLeft(int deadZone)
{
	return (padState.Gamepad.sThumbLX < -deadZone) && (oldPadState.Gamepad.sThumbLX >= -deadZone);
}

bool MyEngine::Input::IsDownLStickRight(int deadZone)
{
	return padState.Gamepad.sThumbLX > deadZone;
}

bool MyEngine::Input::IsTriggerLStickRight(int deadZone)
{
	return (padState.Gamepad.sThumbLX > deadZone) && (oldPadState.Gamepad.sThumbLX <= deadZone);
}

bool MyEngine::Input::IsDownLStickUp(int deadZone)
{
	return padState.Gamepad.sThumbLY > deadZone;
}

bool MyEngine::Input::IsTriggerLStickUp(int deadZone)
{
	return (padState.Gamepad.sThumbLY > deadZone) && (oldPadState.Gamepad.sThumbLY <= deadZone);
}

bool MyEngine::Input::IsDownLStickDown(int deadZone)
{
	return padState.Gamepad.sThumbLY < -deadZone;
}

bool MyEngine::Input::IsTriggerLStickDown(int deadZone)
{
	return (padState.Gamepad.sThumbLY < -deadZone) && (oldPadState.Gamepad.sThumbLY >= -deadZone);
}

bool MyEngine::Input::IsDownRStickLeft(int deadZone)
{
	return padState.Gamepad.sThumbRX < -deadZone;
}

bool MyEngine::Input::IsTriggerRStickLeft(int deadZone)
{
	return (padState.Gamepad.sThumbRX < -deadZone) && (oldPadState.Gamepad.sThumbRX >= -deadZone);
}

bool MyEngine::Input::IsDownRStickRight(int deadZone)
{
	return padState.Gamepad.sThumbRX > deadZone;
}

bool MyEngine::Input::IsTriggerRStickRight(int deadZone)
{
	return (padState.Gamepad.sThumbRX > deadZone) && (oldPadState.Gamepad.sThumbRX <= deadZone);
}

bool MyEngine::Input::IsDownRStickUp(int deadZone)
{
	return padState.Gamepad.sThumbRY > deadZone;
}

bool MyEngine::Input::IsTriggerRStickUp(int deadZone)
{
	return (padState.Gamepad.sThumbRY > deadZone) && (oldPadState.Gamepad.sThumbRY <= deadZone);
}

bool MyEngine::Input::IsDownRStickDown(int deadZone)
{
	return padState.Gamepad.sThumbRY < -deadZone;
}

bool MyEngine::Input::IsTriggerRStickDown(int deadZone)
{
	return (padState.Gamepad.sThumbRY < -deadZone) && (oldPadState.Gamepad.sThumbRY >= -deadZone);
}
