#include "Camera.h"
#include"Base/WinApp.h"
using namespace DirectX;

ID3D12Device* Camera::device = nullptr;

void Camera::StaticInitialize(ID3D12Device* dev)
{
	//NULL�`�F�b�N
	assert(dev);
	device = dev;
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
	result = device->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuff->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	assert(SUCCEEDED(result));

	UpdateMatrix();
}

void Camera::UpdateMatrix()
{
	//��p�̍s���錾
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),					//�㉺��p45�x
		(float)WinApp::winW / WinApp::winH,	//�A�X�y�N�g��i��ʉ���/��ʏc���j
		0.1f, 1000.0f								//�O���A����
	);

	//�r���[�ϊ��s��̌v�Z
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	constMap->view = matView;
	constMap->projection = matProjection;
}

void Camera::Update(XMFLOAT3 playerPos, XMFLOAT3 bossPos)
{
	//�f�[�^�X�V
	this->playerPos_ = playerPos;
	this->bossPos_ = bossPos;

	//���[�h�؂�ւ����N��������
	if (oldMode != mode) {
		//���[�h�؂�ւ����N��������ύX���ɉ��o�����邩�`�F�b�N
		CheckChangeMode();
	}
	else {
		this->oldMode = mode;
	}

	switch (mode) {
	case STRAIGHTMODE:
		UpdateStraightMode();
		break;

	case PLAYERFOLLOWMODE:
		UpdatePlayerFollowMode();
		break;

	case CHANGEMODE:
		UpdateChangeMode();
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
	eye.z += straightModeSpeed;
	target.z = eye.z + 30;
}

void Camera::UpdatePlayerFollowMode()
{
	//�v���C���[�̌�납��v���C���[��Ǐ]���鎋�_
	eye.z = playerPos_.z - 30;
	target.z = playerPos_.z;
}

void Camera::UpdateChangeMode()
{
	//�擾�����C�[�W���O�p�̊J�n�ʒu�ƏI���ʒu�ŃC�[�W���O���s��
	eye = EaseIn3D(nowEye, nextEye, easeing.timeRate);
	target = EaseIn3D(nowTarget, nextTarget, easeing.timeRate);

	easeing.Update();

	if (!easeing.GetActive()) {
		//���o���I������玟�̃��[�h��
		mode = holdMode;
		oldMode = holdMode;
	}
}

void Camera::CheckChangeMode()
{
	//���݂̃��[�h��ێ�
	holdMode = mode;

	//�؂�ւ����ɉ��o������ꍇ�̓C�[�W���O�p�̊J�n�ʒu�ƏI���ʒu���擾

	//�����ړ����[�h����A�v���C���[�Ǐ]���[�h�̐؂�ւ�
	if (oldMode == STRAIGHTMODE && holdMode == PLAYERFOLLOWMODE) {

		//���݂̍��W���J�n�ʒu��
		nowEye = eye;
		nowTarget = target;

		//���̃��[�h�̊J�n�ʒu���v�Z�A�f�[�^�̕ێ�
		holdEye = eye;
		holdTarget = target;
		UpdatePlayerFollowMode();

		//���̃��[�h�̊J�n�ʒu���I���ʒu��
		nextEye = eye;
		nextTarget = target;

		//�f�[�^��߂�
		eye = holdEye;
		target = holdTarget;

		//�؂�ւ����ɉ��o������ꍇCHANGEMODE�ɂ��āA�C�[�W���O�f�[�^�Z�b�g
		//���l�̃��Z�b�g
		easeing.Start(easeingTime);

		mode = CHANGEMODE;
		oldMode = CHANGEMODE;
	}
	else {
		//�����ɂȂ��؂�ւ��̏ꍇ���̂܂ܐ؂�ւ�
		this->oldMode = mode;
	}
}

void Camera::DebugMode()
{

	float speed = 1.0f;

	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_W)) {
			eye.z += speed;
		}
		else if (input_->PushKey(DIK_S)) {
			eye.z -= speed;
		}

		if (input_->PushKey(DIK_A)) {
			eye.x -= speed;
		}
		else if (input_->PushKey(DIK_D)) {
			eye.x += speed;
		}

	}

	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//���W���ړ����鏈��
		if (input_->PushKey(DIK_UP)) {
			target.y += speed;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			target.y -= speed;
		}

		if (input_->PushKey(DIK_RIGHT)) {
			target.x -= speed;
		}
		else if (input_->PushKey(DIK_LEFT)) {
			target.x += speed;
		}

	}
}
