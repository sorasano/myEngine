#include "SpriteManager.h"
using namespace DirectX;
using namespace Microsoft::WRL;
#include<d3dx12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include"GpPipeline.h"
#include"Texture.h"

std::string SpriteManager::defaultTextureDirectoryPath = "";

SpriteManager::SpriteManager()
{
}

SpriteManager::~SpriteManager()
{
}

SpriteManager* SpriteManager::GetInstance()
{
	static SpriteManager instance;
	return &instance;
}

void SpriteManager::Initialize(DirectXCommon* directX, int windowWidth, int windowHeight)
{
	//null�`�F�b�N
	assert(directX);
	//�����������o�ɓn��
	this->directX = directX;
	//�p�C�v���C���X�e�[�g����
	CreatePipeline2D(this->directX->GetDevice());

	//�r���[�s��쐬
	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 0.0f, 1.0f);
}

void SpriteManager::beginDraw()
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	directX->GetCommandList()->SetPipelineState(pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	directX->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`��̐ݒ�
	directX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::descHeap.Get() };
	directX->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void SpriteManager::SetTextureCommand(uint32_t index) {
	//SRV�q�[�v�̐擪�n���h���擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = Texture::descHeap->GetGPUDescriptorHandleForHeapStart();

	UINT incrementSize = directX->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvGpuHandle.ptr += incrementSize * index;
	//SRV�̃n���h�������[�g�p�����[�^1�Ԃɐݒ�
	directX->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

}

void SpriteManager::CreatePipeline2D(ID3D12Device* dev)
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob;		// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;		// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob;	// �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVertexShader.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/SpritePixelShader.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//���_���C�A�E�g�ȂǁA�O���t�B�b�N�X�p�C�v���C�����܂Ƃ߂���̐錾�A�ݒ�
	GpPipeline pipeline;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	inputLayout.push_back(
		{
		"POSITION",										//�Z�}���e�B�b�N��
		0,												//�����Z�}���e�B�b�N�����������鎞�Ɏg���C���f�b�N�X�i0�ł悢�j
		DXGI_FORMAT_R32G32B32_FLOAT,					//�v�f���ƃr�b�g����\���iXYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT�j
		0,												//���̓X���b�g�C���f�b�N�X(0�ł悢)
		D3D12_APPEND_ALIGNED_ELEMENT,					//�f�[�^�̃I�t�Z�b�g�l(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^���(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
		0												//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
		});

	inputLayout.push_back({//uv���W
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		});

	pipeline.SetPipeline(vsBlob.Get(), psBlob.Get(), inputLayout, BLEND_ALPHA);

	//�X�v���C�g�ł͔w�ʃJ�����O���Ȃ�
	pipeline.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipeline.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//�[�x�X�e���V���X�e�[�g��ݒ肵�Ȃ���
	pipeline.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipeline.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;	//�K���㏑���ݒ�
	pipeline.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipeline.desc.DepthStencilState.DepthEnable = false;	//�[�x�e�X�g���s��Ȃ�

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	//�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//���ׂẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//���ׂẴV�F�[�_���猩����
	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[2].Descriptor.ShaderRegister = 1;					//�f�X�N���v�^�����W
	rootParams[2].Descriptor.RegisterSpace = 0;						//�f�X�N���v�^�����W��
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//���ׂẴV�F�[�_���猩����o�b�t�@E

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//���s�J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//���ׂă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;												//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//�s�N�Z���V�F�[�_����̂ݎg�p�\

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);		//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipeline.desc.pRootSignature = rootSignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	pipeline.SetPipelineState(dev, pipelineState);

}
