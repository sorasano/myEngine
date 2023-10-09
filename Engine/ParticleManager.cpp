#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
//const float ParticleManager::radius = 5.0f;				// ��ʂ̔��a
//const float ParticleManager::prizmHeight = 8.0f;			// ���̍���
ID3D12Device* ParticleManager::device_ = nullptr;
UINT ParticleManager::descriptorHandleIncrementSize_ = 0;
//ID3D12GraphicsCommandList* ParticleManager::cmdList = nullptr;
ComPtr<ID3D12RootSignature> ParticleManager::rootsignature_;
ComPtr<ID3D12PipelineState> ParticleManager::pipelinestate_;
//ComPtr<ID3D12DescriptorHeap> ParticleManager::descHeap;
//ComPtr<ID3D12Resource> ParticleManager::vertBuff;
////ComPtr<ID3D12Resource> ParticleManager::indexBuff;
//ComPtr<ID3D12Resource> ParticleManager::texbuff;
//CD3DX12_CPU_DESCRIPTOR_HANDLE ParticleManager::cpuDescHandleSRV;
//CD3DX12_GPU_DESCRIPTOR_HANDLE ParticleManager::gpuDescHandleSRV;
XMMATRIX ParticleManager::matView_{};
XMMATRIX ParticleManager::matProjection_{};
XMFLOAT3 ParticleManager::eye_ = { 0, 0, -5.0f };
XMFLOAT3 ParticleManager::target_ = { 0, 0, 0 };
XMFLOAT3 ParticleManager::up_ = { 0, 1, 0 };
//D3D12_VERTEX_BUFFER_VIEW ParticleManager::vbView{};
//D3D12_INDEX_BUFFER_VIEW ParticleManager::ibView{};
ParticleManager::VertexPos ParticleManager::vertices_[vertexCount];
XMMATRIX ParticleManager::matBillbord_ = XMMatrixIdentity();
XMMATRIX ParticleManager::matBillbordY_ = XMMatrixIdentity();
//
////unsigned short ParticleManager::indices[planeCount * 3];
//
////unsigned short ParticleManager::indices[indexCount];
DirectXCommon* ParticleManager::dx_ = nullptr;

//XMFLOAt3���m�̉��Z����
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs) {
	XMFLOAT3 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

ParticleManager::ParticleManager()
{
	cmdList_ = nullptr;
}

ParticleManager::~ParticleManager()
{
}

void ParticleManager::StaticInitialize(DirectXCommon* dx, int windowWidth, int windowHeight)
{
	// nullptr�`�F�b�N
	assert(dx);

	ParticleManager::dx_ = dx;
	ParticleManager::device_ = dx->GetDevice();

	// �J����������
	InitializeCamera(windowWidth, windowHeight);

	// �p�C�v���C��������
	InitializeGraphicsPipeline();

	// �e�N�X�`���ǂݍ���
	//LoadTexture();

	//// ���f������
	//CreateModel();

}

void ParticleManager::StaticUpdate(XMFLOAT3 eye, XMFLOAT3 target)
{
	ParticleManager::eye_ = eye;
	ParticleManager::target_ = target;
}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(ParticleManager::cmdList_ == nullptr);

	// �R�}���h���X�g���Z�b�g
	ParticleManager::cmdList_ = cmdList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList_->SetPipelineState(pipelinestate_.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList_->SetGraphicsRootSignature(rootsignature_.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleManager::PostDraw()
{
	// �R�}���h���X�g������
	ParticleManager::cmdList_ = nullptr;
}

//ParticleManager* ParticleManager::Create(DirectXCommon* dx_, const std::string& resourcename)
//{
//
//	device = dx_->GetDevice();
//
//	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
//	ParticleManager* particleManager = new ParticleManager();
//	if (particleManager == nullptr) {
//		return nullptr;
//	}
//	
//	LoadTexture(resourcename);
//
//	// ������
//	if (!particleManager->Initialize()) {
//		delete particleManager;
//		assert(0);
//		return nullptr;
//	}
//
//	return particleManager;
//}
void ParticleManager::SetEye(XMFLOAT3 eye)
{
	ParticleManager::eye_ = eye;

	UpdateViewMatrix();
}

void ParticleManager::SetTarget(XMFLOAT3 target)
{
	ParticleManager::target_ = target;

	UpdateViewMatrix();
}

void ParticleManager::CameraMoveVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = GetEye();
	XMFLOAT3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetEye(eye_moved);
	SetTarget(target_moved);
}

void ParticleManager::CameraMoveEyeVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = GetEye();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);

}
void ParticleManager::InitializeDescriptorHeap()
{
	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = 1; // �V�F�[�_�[���\�[�X�r���[1��
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//����
	if (FAILED(result)) {
		assert(0);
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

void ParticleManager::InitializeCamera(int windowWidth, int windowHeight)
{
	//// �r���[�s��̐���
	//matView = XMMatrixLookAtLH(
	//	XMLoadFloat3(&eye),
	//	XMLoadFloat3(&target),
	//	XMLoadFloat3(&up));
	UpdateViewMatrix();


	// ���s���e�ɂ��ˉe�s��̐���
	//constMap->mat = XMMatrixOrthographicOffCenterLH(
	//	0, window_width,
	//	window_height, 0,
	//	0, 1);
	// �������e�ɂ��ˉe�s��̐���
	matProjection_ = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		(float)windowWidth / windowHeight,
		0.1f, 1000.0f
	);
}

void ParticleManager::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> gsBlob;	// �W�I���g���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticleVS.hlsl",	// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticlePS.hlsl",	// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}


	// �W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticleGS.hlsl",	// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "gs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&gsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//�f�v�X�̏������݋֎~
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = device_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootsignature_.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
	assert(SUCCEEDED(result));

}

void ParticleManager::LoadTexture(const std::string& resourcename)
{

	HRESULT result = S_FALSE;

	TexMetadata metadata_{};
	ScratchImage scratchImg_{};

	//���j�R�[�h������ɕϊ�����
	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0,
	resourcename.c_str(), -1, wfilepath, _countof(wfilepath));

	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metadata_, scratchImg_);


	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg_.GetImages(), scratchImg_.GetImageCount(), scratchImg_.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg_ = std::move(mipChain);
		metadata_ = scratchImg_.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata_.format = MakeSRGB(metadata_.format);

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format, metadata_.width, (UINT)metadata_.height, (UINT16)metadata_.arraySize,
		(UINT16)metadata_.mipLevels);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// �e�N�X�`���p�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		nullptr, IID_PPV_ARGS(&texbuff_));
	assert(SUCCEEDED(result));

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	for (size_t i = 0; i < metadata_.mipLevels; i++) {
		const Image* img = scratchImg_.GetImage(i, 0, 0); // ���f�[�^���o
		result = texbuff_->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	// �V�F�[�_���\�[�X�r���[�쐬
	cpuDescHandleSRV_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);
	gpuDescHandleSRV_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap_->GetGPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texbuff_->GetDesc();
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	device_->CreateShaderResourceView(texbuff_.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //�e�N�X�`���ݒ���
		cpuDescHandleSRV_
	);

}

void ParticleManager::CreateModel()
{
	HRESULT result = S_FALSE;

	//for (int i = 0; i < vertexCount; i++) {
	//	const float md_width = 10.0f;
	//	vertices[i].pos.x = (float)rand() / RAND_MAX * md_width - md_width / 2.0f;
	//	vertices[i].pos.y = (float)rand() / RAND_MAX * md_width - md_width / 2.0f;
	//	vertices[i].pos.z = (float)rand() / RAND_MAX * md_width - md_width / 2.0f;
	//}

	//�l�p�`�̃C���f�b�N�X�f�[�^
	unsigned short indicesSquare[] = {
		0,1,2,//�O�p�`1
		2,1,3,//�O�p�`2
	};

	////�����o�ϐ��ɃR�s�[
	//std::copy(std::begin(indicesSquare), std::end(indicesSquare),indices);

	UINT sizeVB = static_cast<UINT>(sizeof(vertices_));

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// ���_�o�b�t�@����
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPos* vertMap_ = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	if (SUCCEEDED(result)) {
		memcpy(vertMap_, vertices_, sizeof(vertices_));
		vertBuff_->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(vertices_);
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	//UINT sizeIB = static_cast<UINT>(sizeof(indices));
	//// ���\�[�X�ݒ�
	//resourceDesc.Width = sizeIB;

	//// �C���f�b�N�X�o�b�t�@����
	//result = device->CreateCommittedResource(
	//	&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	//	IID_PPV_ARGS(&indexBuff));

	//// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	//unsigned short* indexMap = nullptr;
	//result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//if (SUCCEEDED(result)) {

	//	// �S�C���f�b�N�X�ɑ΂���
	//	for (int i = 0; i < _countof(indices); i++)
	//	{
	//		indexMap[i] = indices[i];	// �C���f�b�N�X���R�s�[
	//	}

	//	indexBuff->Unmap(0, nullptr);
	//}

	//// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	//ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//ibView.Format = DXGI_FORMAT_R16_UINT;
	//ibView.SizeInBytes = sizeof(indices);
}

void ParticleManager::UpdateViewMatrix()
{
	//// �r���[�s��̍X�V
	//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMVECTOR eyePosition = XMLoadFloat3(&eye_);
	XMVECTOR targetPosition = XMLoadFloat3(&target_);
	XMVECTOR upVector = XMLoadFloat3(&up_);

	//�J����Z��
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	//0�x�N�g�������O
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	//�x�N�g���𐳋K��
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//�J����X��
	XMVECTOR cameraAxisX;
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//�x�N�g���𐳋K��
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	//�J����Y��
	XMVECTOR cameraAxisY;
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//�x�N�g���𐳋K��
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	//�J������]�s��
	XMMATRIX matCameraRot;
	//�J�������W�n���烏�[���h���W�n�̕ϊ��s��
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	//�]�u�ɂ��t�s����v�Z
	matView_ = XMMatrixTranspose(matCameraRot);

	//���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);

	//1�̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//�r���[�ϊ��s��ɕ��s�ړ�������ݒ�
	matView_.r[3] = translation;

#pragma region �S�����r���{�[�h�s��̌v�Z
	//�r���{�[�h�s��
	matBillbord_.r[0] = cameraAxisX;
	matBillbord_.r[1] = cameraAxisY;
	matBillbord_.r[2] = cameraAxisZ;
	matBillbord_.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma region

#pragma region Y�����r���{�[�h�s��̌v�Z

	//�J����X��,Y��,Z��
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X���͋���
	ybillCameraAxisX = cameraAxisX;
	//Y���̓��[���h���W�n��Y��
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z����X����Y���̊O��
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	//Y�����r���{�[�h�s��
	matBillbordY_.r[0] = ybillCameraAxisX;
	matBillbordY_.r[1] = ybillCameraAxisY;
	matBillbordY_.r[2] = ybillCameraAxisZ;
	matBillbord_.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma region

}

bool ParticleManager::Initialize(const std::string& resourcename)
{

	// �f�X�N���v�^�q�[�v�̏�����
	InitializeDescriptorHeap();

	LoadTexture(resourcename);

	// ���f������
	CreateModel();

	// nullptr�`�F�b�N
	assert(device_);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	HRESULT result;

	// �萔�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	return true;
}

void ParticleManager::Update()
{
	HRESULT result;
	UpdateViewMatrix();


	//�������s�����p�[�e�B�N����S�폜
	particles_.remove_if([](Particle& x) {return x.frame >= x.num_flame; });

	//�S�p�[�e�B�N���X�V
	for (std::forward_list<Particle>::iterator it = particles_.begin(); it != particles_.end(); it++) {

		//�o�߃t���[�������J�E���g
		it->frame++;
		//���x�ɉ����x�����Z
		it->velocity = it->velocity + it->accel;
		//���x�ɂ��ړ�
		it->position_ = it->position_ + it->velocity;

	}

	//���_�o�b�t�@�փf�[�^�]��
	VertexPos* vertMap_ = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	if (SUCCEEDED(result)) {
		//�p�[�e�B�N���̏���1�����f
		for (std::forward_list<Particle>::iterator it = particles_.begin(); it != particles_.end(); it++) {
			//���W
			vertMap_->pos = it->position_;
			//���̒��_��
			vertMap_++;
		}
		vertBuff_->Unmap(0, nullptr);
	}


	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matView_ * matProjection_;	// �s��̍���
	constMap->matbBillboard = matBillbord_;
	constBuff_->Unmap(0, nullptr);
}

void ParticleManager::Draw()
{
	// nullptr�`�F�b�N
	assert(device_);
	//assert(ParticleManager::cmdList);

	this->cmdList_ = dx_->GetCommandList();

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList_->SetPipelineState(pipelinestate_.Get());
	//// ���[�g�V�O�l�`���̐ݒ�
	cmdList_->SetGraphicsRootSignature(rootsignature_.Get());
	//// �v���~�e�B�u�`���ݒ�
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// ���_�o�b�t�@�̐ݒ�
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList_->IASetIndexBuffer(&ibView_);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList_->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV_);
	// �`��R�}���h
	cmdList_->DrawInstanced((UINT)std::distance(particles_.begin(), particles_.end()), 1, 0, 0);

}

void ParticleManager::Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel)
{
	//���X�g�ɗv�f��ǉ�
	particles_.emplace_front();
	//�ǉ������v�f�̎Q��
	Particle& p = particles_.front();
	//�l�̃Z�b�g
	p.position_ = position;
	p.velocity = velocity;
	p.accel = accel;
	p.num_flame = life;
}
