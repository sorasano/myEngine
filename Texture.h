#pragma once
#include<DirectXTex.h>
#include"DirectXCommon.h"
#include<wrl.h>
#include<array>
#include<string>

class Texture {

	private:
	Texture();
	~Texture();

public:
	//�R�s�[�R���X�g���N�^����
	Texture(const Texture& obj) = delete;
	//������Z�q�𖳌�
	Texture& operator=(const Texture& obj) = delete;

	//�C���X�^���X�A�N�Z�X��p�֐�
	static Texture* GetInstance();


	static const size_t spriteSRVCount = 2048;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;			//�f�X�N���v�^�q�[�v
	static std::array< Microsoft::WRL::ComPtr<ID3D12Resource>, spriteSRVCount >texBuffuers;	//�e�N�X�`���o�b�t�@
	static D3D12_RESOURCE_DESC textureResourceDesc;
	static ID3D12Device* dev;

	
public:
	static uint32_t LoadTexture(const wchar_t*fileName = L"NULL");
	static void Initialize(ID3D12Device* device);
	static void CreateSRV(ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
};

