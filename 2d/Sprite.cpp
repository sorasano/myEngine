#include "Sprite.h"
#include "string.h"
#include "WinApp.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")
#define PI 3.1415

using namespace Microsoft::WRL;

//�ÓI�����o�ϐ�
ID3D12Device* Sprite::device = nullptr;
SpriteManager* Sprite::spriteManager = nullptr;
ComPtr<ID3D12RootSignature>Sprite::rootsignature;
ComPtr<ID3D12PipelineState>Sprite::pipelinestate;

void Sprite::Initialize()
{
	HRESULT result;

	//���_�f�[�^
	vertices[0] = { {   0.0f,100.0f, 0.0f },{ 0.0f,1.0f } };	//����
	vertices[1] = { {   0.0f,  0.0f, 0.0f },{ 0.0f,0.0f } };	//����
	vertices[2] = { { 100.0f,100.0f, 0.0f },{ 1.0f,1.0f } };	//�E��
	vertices[3] = { { 100.0f,100.0f, 0.0f },{ 1.0f,1.0f } };	//�E��
	vertices[4] = { {   0.0f,  0.0f, 0.0f },{ 0.0f,0.0f } };	//����
	vertices[5] = { { 100.0f,  0.0f, 0.0f },{ 1.0f,0.0f } };	//�E��

	//���_�f�[�^�̃T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};	//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	ID3D12Resource* vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	/*Vertex* vertMap = nullptr;*/
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];	//���W���R�s�[
	}
	//�q���������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(Vertex);

	//�萔�o�b�t�@�̐ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES v0{};
	v0.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC v1{};
	v1.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	v1.Width = (sizeof(ConstBuffMaterial) + 0xff) & ~0xff;
	v1.Height = 1;
	v1.DepthOrArraySize = 1;
	v1.MipLevels = 1;
	v1.SampleDesc.Count = 1;
	v1.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&v0,
		D3D12_HEAP_FLAG_NONE,
		&v1,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�}�b�s���O
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));

	//�s��p�萔�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES v2{};
	v2.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC v3{};
	v3.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	v3.Width = (sizeof(ConstBuffTransform) + 0xff) & ~0xff;
	v3.Height = 1;
	v3.DepthOrArraySize = 1;
	v3.MipLevels = 1;
	v3.SampleDesc.Count = 1;
	v3.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&v2,
		D3D12_HEAP_FLAG_NONE,
		&v3,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);

	//�萔�o�b�t�@�}�b�s���O
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(result));
	constMapTransform->mat = DirectX::XMMatrixIdentity();
}

void Sprite::Update()
{

	float left = (0.0f - anchorPoint.x) * scale.x;
	float right = (1.0f - anchorPoint.x) * scale.x;
	float top = (0.0f - anchorPoint.y) * scale.y;
	float bottom = (1.0f - anchorPoint.y) * scale.y;

	//���_�f�[�^
	vertices[0] = { {   left,bottom , 0.0f },{ 0.0f,1.0f } };	//����
	vertices[1] = { {  left,  top, 0.0f },{ 0.0f,0.0f } };	//����
	vertices[2] = { { right,bottom , 0.0f },{ 1.0f,1.0f } };	//�E��
	vertices[3] = { { right,bottom , 0.0f },{ 1.0f,1.0f } };	//�E��
	vertices[4] = { {   left,  top, 0.0f },{ 0.0f,0.0f } };	//����
	vertices[5] = { { right,  top, 0.0f },{ 1.0f,0.0f } };	//�E��

	////���_�f�[�^
	//vertices[0] = { {    0.0f, scale.y, 0.0f },{ 0.0f,1.0f } };	//����
	//vertices[1] = { {    0.0f,    0.0f, 0.0f },{ 0.0f,0.0f } };	//����
	//vertices[2] = { { scale.x, scale.y, 0.0f },{ 1.0f,1.0f } };	//�E��
	//vertices[3] = { { scale.x, scale.y, 0.0f },{ 1.0f,1.0f } };	//�E��
	//vertices[4] = { {    0.0f,    0.0f, 0.0f },{ 0.0f,0.0f } };	//����
	//vertices[5] = { { scale.x,    0.0f, 0.0f },{ 1.0f,0.0f } };	//�E��

	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	//�}�e���A��
	constMapMaterial->color = color;

	//�ό`�s��
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	matWorld = DirectX::XMMatrixIdentity();
	//��]
	XMMATRIX matRot;
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(rotation);
	matWorld *= matRot;
	//���s�ړ�
	XMMATRIX matTrans;
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, 0.0f);
	matWorld *= matTrans;

	//����
	constMapTransform->mat = matWorld;
	//2D���W�ɕϊ�
	constMapTransform->mat.r[0].m128_f32[0] = 2.0f / window_width;
	constMapTransform->mat.r[1].m128_f32[1] = -2.0f / window_height;
	constMapTransform->mat.r[3].m128_f32[0] = -1.0f + (position.x / window_width) * 2;
	constMapTransform->mat.r[3].m128_f32[1] = 1.0f - (position.y / window_height) * 2;

	//���o
	if (isflipEase && endFlip == false) {
		FlipOut();
	}
	else if (isSway) {
		Sway();
	}
}

void Sprite::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//�p�C�v���C���A���[�g�V�O�l�`�����Z�b�g
	cmdList->SetPipelineState(pipelinestate.Get());
	cmdList->SetGraphicsRootSignature(rootsignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { spriteManager->GetSrvHeap() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = spriteManager->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
	//�n���h��1���̃T�C�Y
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//�e�N�X�`���̔ԍ��ɍ��킹�ăn���h����i�߂�
	if (textureNum > 0)
	{
		srvGpuHandle.ptr += incrementSize * textureNum;
	}

	//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�ӂɐݒ�
	cmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	//�`��R�}���h
	cmdList->DrawInstanced(_countof(vertices), 1, 0, 0);
}

void Sprite::CreateGraphicsPipeLine()
{
	HRESULT result;

	ID3DBlob* vsBlob = nullptr;	//���_�V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��� �R���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpriteVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	//�G���[�̏ꍇ
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

	//�s�N�Z���V�F�[�_�̓ǂݍ��� �R���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpritePixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	//�G���[�̏ꍇ
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

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//�W���ݒ�

	//���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//�J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//�|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//���ʂ̐ݒ�
	pipelineDesc.BlendState.RenderTarget[0].BlendEnable = true;
	pipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	pipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//�A���t�@�u�����h
	pipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`��
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��ݒ�
	pipelineDesc.NumRenderTargets = 1;	//�`��Ώۂ�1
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//RGBA
	pipelineDesc.SampleDesc.Count = 1;	//1�s�N�Z���ɂ�1��T���v�����O

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParameter[3] = {};
	//�萔�o�b�t�@0��
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].Descriptor.ShaderRegister = 0;
	rootParameter[0].Descriptor.RegisterSpace = 0;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//�e�N�X�`�����W�X�^0��
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorRange;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//�萔�o�b�t�@1��
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[2].Descriptor.ShaderRegister = 1;
	rootParameter[2].Descriptor.RegisterSpace = 0;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParameter;
	rootSignatureDesc.NumParameters = _countof(rootParameter);
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootsignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void Sprite::FlipOut()
{

	float flipInUp = startEasePos.y - flipInRangeUp;
	float flipInDown = startEasePos.y + flipInRangeDown;

	if (initFlip) {

		if (flipInFase == 0) {

			position = EaseOut(startEasePos, { startEasePos.x,flipInUp }, flipOutEase.timeRate);

			if (!flipOutEase.GetActive()) {
				//���̉��o���I������玟�̒i�K��
				flipInFase = 1;
				flipInDown = position.y + flipInRangeDown;

				//���l�̃��Z�b�g
				startEasePos = position;
				flipOutEase.Start(1.0f);
				flipOutEase.timeRate = 0.0f;
			}

		}
		else if (flipInFase == 1) {

			position = EaseOut(startEasePos, { startEasePos.x,flipInDown }, flipOutEase.timeRate);

			if (!flipOutEase.GetActive()) {
				//���̉��o���I������玟�̒i�K��
				flipInFase = 2;

				//���l�̃��Z�b�g
				startEasePos = position;
				flipOutEase.Start(1.0f);
				flipOutEase.timeRate = 0.0f;
			}


		}
		else if (flipInFase == 2) {

			position = EaseOut(startEasePos, { startEasePos.x, -100 }, flipOutEase.timeRate);

			if (!flipOutEase.GetActive()) {
				//���̉��o���I������玟�̒i�K��
				flipInFase = 3;

				//���l�̃��Z�b�g
				//isflipEase = false;
				//initFlip = false;
				endFlip = true;
			}

		}

	}
	else {
		initFlip = true;
		flipInFase = 0;
		flipInUp = position.y - flipInRangeUp;
		flipOutEase.Start(1.0f);
		flipOutEase.timeRate = 0.0f;
		startEasePos = position;
		isSway = false;
	}

	flipOutEase.Update();
}

void Sprite::Sway()
{

	float swayUp = swayCenterPos.y - (swayRange / 2);
	float swayDown = swayCenterPos.y + (swayRange / 2);

	if (initSway) {

		if (isSwayUp) {
			position = EaseOut(startEasePos, { startEasePos.x,swayUp }, swayEase.timeRate);

			if (!swayEase.GetActive()) {
				//��܂ōs�����牺�~�ɕύX
				isSwayUp = false;

				//���l�̃��Z�b�g
				startEasePos = position;
				swayEase.Start(2.0f);
				swayEase.timeRate = 0.0f;
			}
		}
		else {
			position = EaseOut(startEasePos, { startEasePos.x,swayDown }, swayEase.timeRate);

			if (!swayEase.GetActive()) {
				//���܂ōs������㏸�ɕύX
				isSwayUp = true;
				//���l�̃��Z�b�g
				startEasePos = position;
				swayEase.Start(2.0f);
				swayEase.timeRate = 0.0f;
			}
		}

	}
	else {
		initSway = true;
		swayEase.Start(1.0f);
		startEasePos = position;
	}

	swayEase.Update();
}

void Sprite::Reset()
{
	position = InitPosition;

	initFlip = false;
	isflipEase = false;
	endFlip = false;

	initSway = false;
	StartSway(position);

}
