#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "DirectXCommon.h"

using namespace Microsoft::WRL;
using namespace DirectX;

class Model
{
public:
	// ���_�f�[�^�\����
	struct VertexPosNormalUvSkin
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT4 color;
		XMFLOAT2 uv;  // uv���W
	};

	//// �萔�o�b�t�@�p�f�[�^�\����B0
	//struct ConstBufferDataB0
	//{
	//	XMMATRIX mat;	// �R�c�ϊ��s��
	//};
	//// �萔�o�b�t�@�p�f�[�^�\����B1
	//struct ConstBufferDataB1
	//{
	//	XMFLOAT3 ambient;	//�A���r�G���g�W��
	//	float pad1;			//�p�f�B���O
	//	XMFLOAT3 diffuse;	//�f�B�t���[�Y�W��
	//	float pad2;			//�p�f�B���O
	//	XMFLOAT3 specular;	//�X�y�L�����[�W��
	//	float alpha;		//�A���t�@
	//};
	//�}�e���A��
	struct Material
	{
		std::string name;	//�}�e���A����
		XMFLOAT3 ambient;	//�A���r�G���g�e���x
		XMFLOAT3 diffuse;	//�f�B�t���[�Y�e���x
		XMFLOAT3 specular;	//�X�y�L�����[�e���x
		float alpha;	//�A���t�@
		std::string textureFilename;	//�e�N�X�`���t�@�C����
		//�R���X�g���N�^
		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};
public:
	//�V���O���g���C���X�^���X
	Model *GetInstance();
	//������			dxCommon,			obj��mtl�������Ă�t�@�C���̖��O,	�\��t����摜�̖��O
	void Initialize(DirectXCommon* dx_, const std::string& filename , const std::string& resourcename);

	void InitializeVertex(const std::string& filename);	//���_������
	void InitializeDesc();	//�f�X�N���v�^������
	void CompileShader(const wchar_t* file, const wchar_t* file2);	//�V�F�[�_�ǂݍ���
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	void LoadTexture(const std::string& resourcename);
	//�X�V
	void Update();
	//�Q�b�^�[
	Material GetMaterial() { return material; };
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return descHeap_; };
	ComPtr<ID3D12PipelineState> GetPipelinestate() { return pipelinestate_; };
	ComPtr<ID3D12RootSignature> GetRootSignature() { return rootsignature_; };
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv() { return gpuDescHandleSRV_; };
	UINT GetIndicesSize() { return (UINT)sizeof(indices.size()); };
public:
	DirectXCommon* dx_ = nullptr;
	ComPtr<ID3D12Resource> constBuffMaterial_;
public:
	D3D12_RECT scissorRect{};
	D3D12_VIEWPORT viewport{};

	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};		//���_�o�b�t�@�r���[
	uint32_t textureIndex = 0;	//�e�N�X�`���ԍ�

private:
	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;	//���W
		XMFLOAT3 normalize;	//�@���x�N�g��
		XMFLOAT2 uv;	//uv���W
		XMFLOAT4 color = {1,0,0,1};
		Vertex* parent = nullptr;
	};

	struct Indices
	{
		int num;
	};
	//Vertex�֘A
	size_t verticesCount = 24;
	std::vector<VertexPosNormalUvSkin> vertices_;	//�O���ɓn���p�̒��_�f�[�^
	size_t indicesCount = 36;
	std::vector<unsigned short> indices;
	//�C���f�b�N�X�o�b�t�@�֘A
	ComPtr<ID3D12Resource> indexBuff;
	//���_�o�b�t�@������
	ComPtr<ID3D12Resource> vertBuff_;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootsignature_;
	//�p�C�v���C��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};	//�O���t�B�b�N�X�p�C�v���C��
	//�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> pipelinestate_;
	//�}�e���A��
	Material material;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize_ = 0;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
};

