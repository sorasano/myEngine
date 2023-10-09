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
	static ID3D12Device* device_;

	static void StaticInitialize(ID3D12Device* dev);

	struct ConstBufferCamera {
		XMMATRIX view;
		XMMATRIX projection;
	};

	ConstBufferCamera* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

private:

	XMMATRIX matView_ = {};
	XMMATRIX matProjection_ = {};
	XMFLOAT3 eye_ = { 0, 1, 30 };
	XMFLOAT3 target_ = { 0, 0, 0 };
	XMFLOAT3 up_ = { 0, 1, 0 };;

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

	XMMATRIX GetViewProjection()const { return matView_ * matProjection_; }
	XMFLOAT3 GetEye() { return eye_; }
	XMFLOAT3 GetTarget() { return target_; }
	int GetMode() { return mode_; }
	float GetRangeMaxZ() { return rangeMaxZ_; }
	bool GetIsPerformance() { return isPerformance_; }

	void SetEye(XMFLOAT3 eye) { this->eye_ = eye; }
	void SetTarget(XMFLOAT3 target) { this->target_ = target; }
	void SetMode(int mode);

private:

	//����
	Input* input_ = nullptr;

	float angle_ = 0.1f; // �J�����̉�]�p

	//�v���C���[���W
	XMFLOAT3 playerPos_ = {};
	//�{�X���W
	XMFLOAT3 bossPos_ = {};

	//�`��ő勗��
	float rangeMaxZ_ = 500.0f;
	//�v���C���[�ƃJ�����̋���
	float playerRange_ = 30.0f;

	//-----�J�������[�h-----
	int mode_ = STRAIGHTMODE;

	//�J���������ړ����[�h�X�s�[�h
	float straightModeSpeed_ = 1.0f;

	//-----���o�p-----
	//���ݍ��W
	XMFLOAT3 startEye_ = {};
	XMFLOAT3 startTarget_ = {};

	//�C�[�W���O�I���ʒu
	XMFLOAT3 endEye_ = {};
	XMFLOAT3 endTarget_ = {};

	//���݂̃f�[�^�ۑ��p
	XMFLOAT3 holdEye_ = {};
	XMFLOAT3 holdTarget_ = {};

	//�C�[�W���O���o�p�f�[�^
	Easing easeing_;
	//���o����
	float easeingTime_ = 3.0f;
	//���o����
	bool isPerformance_ = false;
	//�t�F�[�Y
	int phase_ = 0;
};