#include "SpriteManager.h"


//静的メンバ変数
ID3D12Device* SpriteManager::device = nullptr;

void SpriteManager::Initialize()
{
	HRESULT result;

	metadata.resize(kMaxSrvCount);
	scratchImg.resize(kMaxSrvCount);

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSrvCount;

	//設定を元にSRV用デスクリプタヒープを生成
	/*ID3D12DescriptorHeap* srvHeap = nullptr;*/
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}

void SpriteManager::LoadFile(int number, const wchar_t* fileName)
{
	HRESULT result;

	/*DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};*/

	//WICテクスチャのロード
	result = DirectX::LoadFromWICFile(
		fileName,
		DirectX::WIC_FLAGS_NONE,
		&metadata[number],
		scratchImg[number]
	);

	DirectX::ScratchImage mipChain{};

	//ミップマップ生成
	result = DirectX::GenerateMipMaps(
		scratchImg[number].GetImages(),
		scratchImg[number].GetImageCount(),
		scratchImg[number].GetMetadata(),
		DirectX::TEX_FILTER_DEFAULT,
		0,
		mipChain
	);
	if (SUCCEEDED(result))
	{
		scratchImg[number] = std::move(mipChain);
		metadata[number] = scratchImg[number].GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata[number].format = DirectX::MakeSRGB(metadata[number].format);

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata[number].format;
	textureResourceDesc.Width = metadata[number].width;
	textureResourceDesc.Height = (UINT)metadata[number].height;
	textureResourceDesc.DepthOrArraySize = (UINT)metadata[number].arraySize;
	textureResourceDesc.MipLevels = (UINT)metadata[number].mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//テクスチャバッファ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//テクスチャバッファの生成
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuff[number])
	);

	for (size_t i = 0; i < metadata[number].mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const DirectX::Image* img = scratchImg[number].GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureBuff[number]->WriteToSubresource(
			(UINT)i,
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch
		);
		assert(SUCCEEDED(result));
	}

	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//ハンドル1分のサイズ
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (number > 0)
	{
		srvHandle.ptr += (incrementSize * number);
	}

	//シェーダリソースビューの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダリソースビュー作成
	device->CreateShaderResourceView(textureBuff[number].Get(), &srvDesc, srvHandle);

}
