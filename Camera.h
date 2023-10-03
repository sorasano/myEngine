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
	TITLETOPLAYMODE,//�^�C�g������v���C�J��
	BOSSINMODE,//�{�X�o�����o
	BOSSCLERAMODE,//�{�X���j���o
	BOSSGAMEOVERAMODE,//�{�X���S���o
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

	//�^�C�g������v���C�J�ڍX�V
	void UpdateTitleToPlayMode();
	//�^�C�g������v���C�J�ڏ�����
	void InitializeTitleToPlayMode();

	//�f�o�b�N���[�h
	void DebugMode();

	XMMATRIX GetViewProjection()const { return matView * matProjection; }
	XMFLOAT3 GetEye() { return eye; }
	XMFLOAT3 GetTarget() { return target; }
	int GetMode() { return mode; }
	float GetRangeMaxZ() { return rangeMaxZ; }
	bool GetIsPerformance() { return isPerformance; }

	void SetEye(XMFLOAT3 eye) { this->eye = eye; }
	void SetTarget(XMFLOAT3 target) { this->target = target; }
	void SetMode(int mode);

private:

	//����
	Input* input_ = nullptr;

	float angle = 0.1f; // �J�����̉�]�p

	//�v���C���[���W
	XMFLOAT3 playerPos_ = {};
	//�{�X���W
	XMFLOAT3 bossPos_ = {};

	//�`��ő勗��
	float rangeMaxZ = 500.0f;
	//�v���C���[�ƃJ�����̋���
	float playerRange = 30.0f;

	//-----�J�������[�h-----
	int mode = STRAIGHTMODE;

	//�J���������ړ����[�h�X�s�[�h
	float straightModeSpeed = 1.0f;

	//-----���o�p-----
	//���ݍ��W
	XMFLOAT3 startEye = {};
	XMFLOAT3 startTarget = {};

	//�C�[�W���O�I���ʒu
	XMFLOAT3 endEye = {};
	XMFLOAT3 endTarget = {};

	//���݂̃f�[�^�ۑ��p
	XMFLOAT3 holdEye = {};
	XMFLOAT3 holdTarget = {};

	//�C�[�W���O���o�p�f�[�^
	Easing easeing;
	//���o����
	float easeingTime = 3.0f;
	//���o����
	bool isPerformance = false;
	//�t�F�[�Y
	int phase = 0;
};