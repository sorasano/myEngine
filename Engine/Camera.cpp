#include "Camera.h"
#include"Base/WinApp.h"
using namespace DirectX;

ID3D12Device* Camera::device_ = nullptr;

void Camera::StaticInitialize(ID3D12Device* dev)
{
	//NULL�`�F�b�N
	assert(dev);
	device_ = dev;
}

void Camera::Initialize(Input* input)
{
	//�����������o�Ɉڂ��čs��X�V
	this->input_ = input;

	HRESULT result;
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferCamera) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuff_->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	assert(SUCCEEDED(result));

	UpdateMatrix();
}

void Camera::UpdateMatrix()
{
	//��p�̍s���錾
	matProjection_ = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),					//�㉺��p45�x
		(float)WinApp::winW / WinApp::winH,	//�A�X�y�N�g��i��ʉ���/��ʏc���j
		0.1f, 1000.0f								//�O���A����
	);

	//�r���[�ϊ��s��̌v�Z
	matView_ = XMMatrixLookAtLH(XMLoadFloat3(&eye_), XMLoadFloat3(&target_), XMLoadFloat3(&up_));

	constMap->view = matView_;
	constMap->projection = matProjection_;
}

void Camera::Update(XMFLOAT3 playerPos, XMFLOAT3 bossPos)
{
	//�f�[�^�X�V
	this->playerPos_ = playerPos;
	this->bossPos_ = bossPos;

	switch (mode_) {
	case STRAIGHTMODE:
		UpdateStraightMode();
		break;

	case PLAYERFOLLOWMODE:
		UpdatePlayerFollowMode();
		break;

	case TITLETOPLAYMODE:
		UpdateTitleToPlayMode();
		break;

	case DEBUGMODE:
		DebugMode();
		break;
	}

	UpdateMatrix();
}

void Camera::UpdateStraightMode()
{
	//���X�s�[�h�Ői�ݑ�����
	eye_.z += straightModeSpeed_;
	target_.z = eye_.z + playerRange_;
}

void Camera::UpdatePlayerFollowMode()
{
	//�v���C���[�̌�납��v���C���[��Ǐ]���鎋�_
	eye_.z = playerPos_.z - playerRange_;
	target_.z = playerPos_.z;
}

void Camera::UpdateTitleToPlayMode()
{
	if (phase_ == 0) {

		//�擾�����C�[�W���O�p�̊J�n�ʒu�ƏI���ʒu�ŃC�[�W���O���s��
		eye_ = EaseIn3D(startEye_, endEye_, easeing_.timeRate);
		target_ = EaseIn3D(startTarget_, endTarget_, easeing_.timeRate);

		if (!easeing_.GetActive()) {
			//���o���I������玟�̃t�F�[�Y��
			phase_++;

			//�C�[�W���O�p�̃f�[�^��ݒ肵�Ȃ���
			easeing_.Start(easeingTime_);

			startEye_ = eye_;
			startTarget_ = target_;

			endEye_ = holdEye_;
			endTarget_ = holdTarget_;
		}
	}
	else if (phase_ == 1) {

		//�擾�����C�[�W���O�p�̊J�n�ʒu�ƏI���ʒu�ŃC�[�W���O���s��
		eye_ = EaseIn3D(startEye_, endEye_, easeing_.timeRate);
		target_ = EaseIn3D(startTarget_, endTarget_, easeing_.timeRate);

		if (!easeing_.GetActive()) {
			//���o���I������烂�[�h�̐؂�ւ�
			mode_ = PLAYERFOLLOWMODE;
			isPerformance_ = false;
		}
	}

	easeing_.Update();
}

void Camera::InitializeTitleToPlayMode()
{

	//�C�[�W���O�p�̊J�n�ʒu�ƏI���ʒu���擾

	//���݂̍��W���J�n�ʒu��
	startEye_ = eye_;
	startTarget_ = target_;

	//�������W��ێ�
	holdEye_ = startEye_;
	holdTarget_ = startTarget_;

	//���݂̕`��ő勗�����I���ʒu��
	endEye_ = eye_;
	endTarget_ = target_;

	endEye_.z = eye_.z + (rangeMaxZ_  * 2);
	endTarget_.z = eye_.z + (rangeMaxZ_ * 2) + playerRange_;

	//�C�[�W���O�p���l�̏�����
	easeing_.Start(easeingTime_);
	phase_ = 0;

	//�p�t�H�[�}���X�t���O
	isPerformance_ = true;
}

void Camera::DebugMode()
{

	float speed = 1.0f;

	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_W)) {
			eye_.z += speed;
		}
		else if (input_->PushKey(DIK_S)) {
			eye_.z -= speed;
		}

		if (input_->PushKey(DIK_A)) {
			eye_.x -= speed;
		}
		else if (input_->PushKey(DIK_D)) {
			eye_.x += speed;
		}

	}

	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_UP)) {
			target_.y += speed;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			target_.y -= speed;
		}

		if (input_->PushKey(DIK_RIGHT)) {
			target_.x -= speed;
		}
		else if (input_->PushKey(DIK_LEFT)) {
			target_.x += speed;
		}

	}
}

void Camera::SetMode(int mode)
{
	this->mode_ = mode;

	//���[�h���Ƃɏ��������K�v�ȏꍇ�͏�����
	switch (mode_) {
	case TITLETOPLAYMODE:
		InitializeTitleToPlayMode();
		break;
	case STRAIGHTMODE:
		break;
	case BOSSINMODE:
		break;
	case BOSSCLERAMODE:
		break;
	case BOSSGAMEOVERAMODE:
		break;
	}
}
