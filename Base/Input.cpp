#include "Input.h"

void Input::Initialize(WinApp* winApp) {
	
	HRESULT result;

	//�؂�Ă���WinApp�̃C���X�^���X���L�^
	this->winApp_ = winApp;

	//�L�[�{�[�h����

// DirectInput�̏�����

	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// �L�[�{�[�h�f�o�C�X�̐���

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// ���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��
	assert(SUCCEEDED(result));

	// �r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

}

void Input::Update() {

	// �L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();

	//// �ŐV�̃L�[�{�[�h��񂾂������̂�1�t���[���O�̃L�[�{�[�h���Ƃ��ĕۑ�
	//for (int i = 0; i < 256; i++) {
	//	oldkey[i] = key[i];
	//}

	//�O��̃L�[���͂�ۑ�
	memcpy(keyPre, key, sizeof(key));

	// �S�L�[�̓��͏�Ԃ��擾����
	keyboard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber)
{
	//�w��̃L�[�������Ă����true��Ԃ�
	if (key[keyNumber]) {
		return true;
	}

	//�����łȂ������false��Ԃ�
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	//�w��̃L�[�������Ă����true��Ԃ�
	if (key[keyNumber] && keyPre[keyNumber] == false) {
		return true;
	}

	return false;
}
