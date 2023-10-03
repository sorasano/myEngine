#pragma once
#include"Base/DirectXCommon.h"
#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>

#include"Input.h"
#include"Easing.h"

using namespace DirectX;

enum CameraMode {
	STRAIGHTMODE,//�����ړ�
	PLAYERFOLLOWMODE,//���@�ɒǏ]
	BOSSAPPEAREARANCEMODE,//�{�X�o�����o
	BOSSDESTROYMODE,//�{�X���j���o
	CHANGEMODE,//���[�h�̐؂�ւ�
	DEBUGMODE,//�f�o�b�N���[�h
};

class Camera
{
public:
	static ID3D12Device* device;

	static void StaticInitialize(ID3D12Device* dev);

	struct ConstBufferCamera {
		XMMATRIX view;
		XMMATRIX projection;
	};

	ConstBufferCamera* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	XMMATRIX matView = {};
	XMMATRIX matProjection = {};
	XMFLOAT3 eye = { 0, 1, 30 };
	XMFLOAT3 target = { 0, 0, 0 };
	XMFLOAT3 up = { 0, 1, 0 };;

public:


	void Initialize(Input* input);

	void UpdateMatrix();

	void Update(XMFLOAT3 playerPos, XMFLOAT3 bossPos);

	//�����ړ����[�h
	void UpdateStraightMode();
	//�v���C���[�Ǐ]���[�h
	void UpdatePlayerFollowMode();

	//���[�h�ύX���[�h
	void UpdateChangeMode();
	//���[�h�ύX���[�h�`�F�b�N
	void CheckChangeMode();

	//�f�o�b�N���[�h
	void DebugMode();

	XMMATRIX GetViewProjection()const { return matView * matProjection; }
	XMFLOAT3 GetEye() { return eye; }
	XMFLOAT3 GetTarget() { return target; }
	int GetMode() { return mode; }

	void SetEye(XMFLOAT3 eye) { this->eye = eye; }
	void SetTarget(XMFLOAT3 target) { this->target = target; }
	void SetMode(int mode) { this->mode = mode; }

private:

	//����
	Input* input_ = nullptr;

	float angle = 0.1f; // �J�����̉�]�p

	//�v���C���[���W
	XMFLOAT3 playerPos_ = {};
	//�{�X���W
	XMFLOAT3 bossPos_ = {};

	//-----�J�������[�h-----
	int mode = STRAIGHTMODE;
	//�O�t���[���̃��[�h
	int oldMode = STRAIGHTMODE;
	//���݂̃��[�h�ێ��p
	int holdMode = STRAIGHTMODE;


	//�J���������ړ����[�h�X�s�[�h
	float straightModeSpeed = 1.0f;

	//-----�J�������[�h�ύX�����o�p-----
	//���ݍ��W
	XMFLOAT3 nowEye = {};
	XMFLOAT3 nowTarget = {};

	//���̃��[�h�J�n�ʒu
	XMFLOAT3 nextEye = {};
	XMFLOAT3 nextTarget = {};

	//���݂̃f�[�^�ۑ��p
	XMFLOAT3 holdEye = {};
	XMFLOAT3 holdTarget = {};

	//�C�[�W���O���o�p�f�[�^
	Easing easeing;
	//���o����
	float easeingTime = 1.0f;
};