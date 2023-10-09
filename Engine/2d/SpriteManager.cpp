#include "SpriteManager.h"
const std::string SpriteManager::kDefaultbaseDirectory_ = "Resources/texture/";

//�ÓI�����o�ϐ�
ID3D12Device* SpriteManager::device_ = nullptr;

void SpriteManager::Initialize()
{
	HRESULT result;

	metadata_.resize(kMaxSrvCount);
	scratchImg_.resize(kMaxSrvCount);

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSrvCount;

	//�ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	/*ID3D12DescriptorHeap* srvHeap = nullptr;*/
	result = device_->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));
}

void SpriteManager::LoadFile(int number, const std::string fileName)
{
	HRESULT result;

	//�A�����ăt���p�X�𓾂�
	std::string fullpath = kDefaultbaseDirectory_ + fileName;

	//string->wchar�ϊ�
	const size_t newsizew = fullpath.size() + 1;
	size_t convertedChars = 0;
	wchar_t* wcstring = new wchar_t[newsizew];
	mbstowcs_s(&convertedChars, wcstring, newsizew, fullpath.c_str(), _TRUNCATE);

	//WIC�e�N�X�`���̃��[�h
	result = DirectX::LoadFromWICFile(
		wcstring,
		DirectX::WIC_FLAGS_NONE,
		&metadata_[number],
		scratchImg_[number]
	);

	delete[]wcstring;

	DirectX::ScratchImage mipChain{};

	//�~�b�v�}�b�v����
	result = DirectX::GenerateMipMaps(
		scratchImg_[number].GetImages(),
		scratchImg_[number].GetImageCount(),
		scratchImg_[number].GetMetadata(),
		DirectX::TEX_FILTER_DEFAULT,
		0,
		mipChain
	);
	if (SUCCEEDED(result))
	{
		scratchImg_[number] = std::move(mipChain);
		metadata_[number] = scratchImg_[number].GetMetadata();
	}

	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata_[number].format = DirectX::MakeSRGB(metadata_[number].format);

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata_[number].format;
	textureResourceDesc.Width = metadata_[number].width;
	textureResourceDesc.Height = static_cast<UINT16>(metadata_[number].height);
	textureResourceDesc.DepthOrArraySize = static_cast<UINT16>(metadata_[number].arraySize);
	textureResourceDesc.MipLevels = static_cast<UINT16>(metadata_[number].mipLevels);
	textureResourceDesc.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//�e�N�X�`���o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuff_[number])
	);

	for (size_t i = 0; i < metadata_[number].mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const DirectX::Image* img = scratchImg_[number].GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = textureBuff_[number]->WriteToSubresource(
			(UINT)i,
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch
		);
		assert(SUCCEEDED(result));
	}

	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
	//�n���h��1���̃T�C�Y
	UINT incrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (number > 0)
	{
		srvHandle.ptr += (incrementSize * number);
	}

	//�V�F�[�_���\�[�X�r���[�̍쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_���\�[�X�r���[�쐬
	device_->CreateShaderResourceView(textureBuff_[number].Get(), &srvDesc, srvHandle);

}
