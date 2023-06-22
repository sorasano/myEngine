#include "Texture.h"
#include<string.h>
#include<cassert>
#include"Random.h"

using namespace DirectX;
using namespace Microsoft::WRL;


ComPtr<ID3D12DescriptorHeap>Texture::descHeap;			//�f�X�N���v�^�q�[�v
std::array<ComPtr<ID3D12Resource>, Texture::spriteSRVCount >Texture::texBuffuers;	//�e�N�X�`���o�b�t�@
D3D12_RESOURCE_DESC Texture::textureResourceDesc{};
ID3D12Device* Texture::dev = nullptr;

Texture::Texture()
{
}

Texture::~Texture()
{
}

Texture* Texture::GetInstance()
{
	static Texture instance;
	return &instance;
}

uint32_t Texture::LoadTexture(const wchar_t* fileName)
{
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};
	DirectX::ScratchImage mipChain{};

	HRESULT result;
	result = LoadFromWICFile(
		fileName,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;

	//�t�@�C��������������ʏ�̃t�@�C���ǂݍ���
	if (fileName != L"NULL") {
		//�~�b�v�}�b�v����
		result = GenerateMipMaps(
			scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
			TEX_FILTER_DEFAULT, 0, mipChain);
		if (SUCCEEDED(result)) {
			scratchImg = std::move(mipChain);
			metadata = scratchImg.GetMetadata();
		}
		// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
		metadata.format = MakeSRGB(metadata.format);
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES textureHeapProp{};
		textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		textureHeapProp.CPUPageProperty =
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		//���\�[�X�ݒ�
		textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureResourceDesc.Format = metadata.format;
		textureResourceDesc.Width = metadata.width;	// ��
		textureResourceDesc.Height = (UINT)metadata.height;	// ����
		textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
		textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
		textureResourceDesc.SampleDesc.Count = 1;
		result = dev->CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texBuff));
		//�S�~�b�v�}�b�v�ɂ���
		for (size_t i = 0; i < metadata.mipLevels; i++) {
			//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
			const Image* img = scratchImg.GetImage(i, 0, 0);
			//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
			result = texBuff->WriteToSubresource(
				(UINT)i,
				nullptr,				//�S�̈�փR�s�[
				img->pixels,			//���f�[�^�A�h���X
				(UINT)img->rowPitch,	//1���C���T�C�Y
				(UINT)img->slicePitch	//�S�T�C�Y
			);
			assert(SUCCEEDED(result));
		}
	}
	else {	//�t�@�C����������Ȃ�or�t�@�C�������ǂݍ��܂�Ă��Ȃ��Ȃ�256*256�̃����_���̐F�̃s�N�Z���̉摜�f�[�^������
		////�������s�N�Z����
		const size_t textureWidth = 256;
		//�c�����s�N�Z����
		const size_t textureHeight = 256;
		//�z��̗v�f��
		const size_t imageDataCount = textureWidth * textureHeight;
		//�摜�C���[�W�f�[�^�z��
		XMFLOAT4* imageData = new XMFLOAT4[imageDataCount]; //���K����ŊJ������

		//�S�s�N�Z���̐F��������
		for (size_t i = 0; i < imageDataCount; i++) {
			imageData[i].x = Random(0.0f, 1.0f);	// R
			imageData[i].y = Random(0.0f, 1.0f); // G
			imageData[i].z = Random(0.0f, 1.0f); // B
			imageData[i].w = Random(0.0f, 1.0f); // A
		}

		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES textureHeapProp{};
		textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		textureHeapProp.CPUPageProperty =
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		//���\�[�X�ݒ�
		textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureResourceDesc.Width = textureWidth;	// ��
		textureResourceDesc.Height = textureHeight;	// ����
		textureResourceDesc.DepthOrArraySize = 1;
		textureResourceDesc.MipLevels = 1;
		textureResourceDesc.SampleDesc.Count = 1;

		result = dev->CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texBuff));

		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(
			0,
			nullptr,				//�S�̈�փR�s�[
			imageData,			//���f�[�^�A�h���X
			sizeof(XMFLOAT4) * textureWidth,	//1���C���T�C�Y
			sizeof(XMFLOAT4) * imageDataCount 	//�S�T�C�Y
		);
		delete[] imageData;
	}

	for (int i = 0; i < texBuffuers.size(); i++) {

		//�܂��e�N�X�`����񂪊��蓖�Ă��Ă��Ȃ��Ƃ���Ƀe�N�X�`����������
		if (!texBuffuers[i]) {
			texBuffuers[i] = texBuff;
			//SRV�쐬
			UINT incrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = descHeap->GetCPUDescriptorHandleForHeapStart();
			srvHandle.ptr += incrementSize * i;
			CreateSRV(texBuff.Get(), srvHandle);
			//i��߂�l�Ƃ��ĕԂ�
			return i;
		}
	}

}

void Texture::Initialize(ID3D12Device* device)
{
	HRESULT result;
	//�f�o�C�X�̃C���X�^���X���؂��
	dev = device;

	//�f�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = dev->CreateDescriptorHeap(&descHeapDesc,
		IID_PPV_ARGS(&descHeap));
	assert(SUCCEEDED(result));

}

void Texture::CreateSRV(ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	dev->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);
}
