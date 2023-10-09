#pragma once
#include "FbxModel.h"
#include "Camera.h"

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string.h>

class FbxObject3D
{
public: //�萔
	//�{�[���̍ő吔
	static const int MAX_BONES = 32;
public:
	//�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};

	//�萔�o�b�t�@�p�f�[�^�\����(�X�L�j���O)
	struct ConstBufferDataSkin {
		XMMATRIX bones[MAX_BONES];
	};

private:	//�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:	//�ÓI�����o�֐�
	//�Z�b�^�[
	static void SetDevice(ID3D12Device* device) { FbxObject3D::device_ = device; }
	static void SetCamera(Camera* camera) { FbxObject3D::camera_ = camera; }

	//�O���t�B�b�N�p�C�v���C���̐���
	static void CreateGraphicsPipeline();

private://�ÓI�����o�ϐ�
	static ID3D12Device* device_;
	static Camera* camera_;

	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature_;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate_;

public://�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//���f���̃Z�b�g
	void SetModel(FbxModel* model) { this->model_ = model; }

	//�A�j���[�V�����J�n
	void PlayAnimation();

	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position_ = position; }
	void SetRotate(XMFLOAT3 rotate) { this->rotation_ = rotate; }
	void SetScale(XMFLOAT3 scale) { this->scale_ = scale; }

	//�Q�b�^�[
	XMFLOAT3 GetPosition() { return this->position_; }
	XMFLOAT3 GetRotate() { return this->rotation_ ; }
	XMFLOAT3 GetScale() { return this->scale_ ; }

protected://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource>constBuffTransform_;
	//�萔�o�b�t�@(�X�L��)
	ComPtr<ID3D12Resource> constBuffSkin_;

private:

	//���[�J���X�P�[��
	XMFLOAT3 scale_ = { 1,1,1 };
	//X,Y,Z�����̃��[�J���s��
	XMFLOAT3 rotation_ = { 0,0,0 };
	//���[�J�����W
	XMFLOAT3 position_ = { 0,0,0 };
	//���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld_ = {};
	//���f��
	FbxModel* model_ = nullptr;

	//1�t���[���̎���
	FbxTime frameTime_;
	//�A�j���[�V�����J�n����
	FbxTime startTime_;
	//�A�j���[�V�����I������
	FbxTime endTime_;
	//���ݎ���
	FbxTime currentTime_;
	//�A�j���[�V�����Đ���
	bool isPlay_ = false;
};

