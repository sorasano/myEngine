#pragma once
#include "d3dx12.h"
#include "DirectXMath.h"
#include "array"

#include <DirectXTex.h>

#include "vector"

class SpriteManager
{
private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:	 //定数
	//SRVの最大個数
	static const size_t kMaxSrvCount = 2056;

public:	//メンバ関数
	void Initialize();
	void LoadFile(int number, const std::string fileName);
	void SetTextureCommand(int number);

public:	//ゲッター
	ID3D12Resource* GetTextureBuff(int number) { return textureBuff_[number].Get(); }
	ID3D12DescriptorHeap* GetSrvHeap() { return srvHeap_.Get(); }

public:
	static void SetDevice(ID3D12Device* device) { SpriteManager::device_ = device; }

private:	//静的メンバ変数
	//デバイス
	static ID3D12Device* device_;

private:	//メンバ変数
	//テクスチャバッファ
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSrvCount>textureBuff_;
	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;
	std::vector<DirectX::TexMetadata> metadata_;
	std::vector<DirectX::ScratchImage> scratchImg_;

	//ファイル格納ルートパス
	static const std::string kDefaultbaseDirectory_;
};

