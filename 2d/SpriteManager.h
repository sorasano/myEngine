#pragma once
#include "d3dx12.h"
#include "DirectXMath.h"
#include "array"
#include "DirectXTex.h"
#include "vector"

class SpriteManager
{
private:	//�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:	 //�萔
	//SRV�̍ő��
	static const size_t kMaxSrvCount = 2056;

public:	//�����o�֐�
	void Initialize();
	void LoadFile(int number, const std::string fileName);
	void SetTextureCommand(int number);

public:	//�Q�b�^�[
	ID3D12Resource* GetTextureBuff(int number) { return textureBuff[number].Get(); }
	ID3D12DescriptorHeap* GetSrvHeap() { return srvHeap.Get(); }

public:
	static void SetDevice(ID3D12Device* device) { SpriteManager::device = device; }

private:	//�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device;

private:	//�����o�ϐ�
	//�e�N�X�`���o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSrvCount>textureBuff;
	//�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	std::vector<DirectX::TexMetadata> metadata;
	std::vector<DirectX::ScratchImage> scratchImg;

	//�t�@�C���i�[���[�g�p�X
	static const std::string kDefaultbaseDirectory;
};

