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

void Camera::Initialize(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up, Input* input)
{
	//�����������o�Ɉڂ��čs��X�V
	this->eye = eye;
	this->target = target;
	this->up = up;
	this->input = input;



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

void Camera::Update()
{

	UpdateTarget();

	UpdateEye();

	UpdateMatrix();

}

void Camera::UpdateEye()
{
	//�ˉe�ϊ�

	//if (input->PushKey(DIK_D) || input->PushKey(DIK_A)) {

	//	if (input->PushKey(DIK_D)) { angle += XMConvertToRadians(1.0f); }
	//	else if (input->PushKey(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

	//	//angle���W�A������Y�����ɉ�].���a��-100
	//	eye.x = -100 * sinf(angle);
	//	//eye.z = -10 * cosf(angle);

	//}

	if (input->PushKey(DIK_A)) {
		eye.x += 1;
	}
	if (input->PushKey(DIK_D)) {
		eye.x -= 1;
	}

	if (input->PushKey(DIK_W)) {
		eye.y += 1;
	}
	if (input->PushKey(DIK_S)) {
		eye.y -= 1;
	}

}

void Camera::UpdateTarget()
{

}