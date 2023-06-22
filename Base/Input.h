#include <cassert>

#include <wrl.h>

using namespace Microsoft::WRL;

#pragma once
#define DIRECTINPUT_VERSION     0x0800   // DirectInput�̃o�[�W�����w��
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <Windows.h>
#include "WinApp.h"

class Input
{
public:

	//namespace�ȗ�
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;

public:

	void Initialize(WinApp* winApp);

	void Update();

	//�L�[�̉������`�F�b�N
	bool PushKey(BYTE keyNumber);

	//�L�[�̃g���K�[���`�F�b�N
	bool TriggerKey(BYTE keyNumber);

private:

	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInput8> directInput;

	BYTE key[256] = {};
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};

