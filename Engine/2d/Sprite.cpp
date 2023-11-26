/**
* @file Sprite.cpp
* @brief スプライトの処理
*/

#include "Sprite.h"
#include "string.h"
#include "WinApp.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>


#pragma comment(lib, "d3dcompiler.lib")
#define PI 3.1415

using namespace Microsoft::WRL;

//静的メンバ変数
ID3D12Device* Sprite::device_ = nullptr;
SpriteManager* Sprite::spriteManager_ = nullptr;
ComPtr<ID3D12RootSignature>Sprite::rootsignature_;
ComPtr<ID3D12PipelineState>Sprite::pipelinestate_;

void Sprite::Initialize()
{
	HRESULT result;

	//頂点データ
	vertices_[0] = { {   0.0f,100.0f, 0.0f },{ 0.0f,1.0f } };	//左下
	vertices_[1] = { {   0.0f,  0.0f, 0.0f },{ 0.0f,0.0f } };	//左上
	vertices_[2] = { { 100.0f,100.0f, 0.0f },{ 1.0f,1.0f } };	//右下
	vertices_[3] = { { 100.0f,100.0f, 0.0f },{ 1.0f,1.0f } };	//右下
	vertices_[4] = { {   0.0f,  0.0f, 0.0f },{ 0.0f,0.0f } };	//左上
	vertices_[5] = { { 100.0f,  0.0f, 0.0f },{ 1.0f,0.0f } };	//右上

	//頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices_));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ID3D12Resource* vertBuff_ = nullptr;
	result = device_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	/*Vertex* vertMap = nullptr;*/
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices_); i++)
	{
		vertMap_[i] = vertices_[i];	//座標をコピー
	}
	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);

	//頂点バッファビューの作成
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(Vertex);

	//定数バッファの設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES v0{};
	v0.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC v1{};
	v1.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	v1.Width = (sizeof(ConstBuffMaterial) + 0xff) & ~0xff;
	v1.Height = 1;
	v1.DepthOrArraySize = 1;
	v1.MipLevels = 1;
	v1.SampleDesc.Count = 1;
	v1.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&v0,
		D3D12_HEAP_FLAG_NONE,
		&v1,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_)
	);
	assert(SUCCEEDED(result));

	//定数バッファマッピング
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);
	assert(SUCCEEDED(result));

	//行列用定数バッファ設定
	D3D12_HEAP_PROPERTIES v2{};
	v2.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC v3{};
	v3.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	v3.Width = (sizeof(ConstBuffTransform) + 0xff) & ~0xff;
	v3.Height = 1;
	v3.DepthOrArraySize = 1;
	v3.MipLevels = 1;
	v3.SampleDesc.Count = 1;
	v3.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&v2,
		D3D12_HEAP_FLAG_NONE,
		&v3,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform_)
	);

	//定数バッファマッピング
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);
	assert(SUCCEEDED(result));
	constMapTransform_->mat = DirectX::XMMatrixIdentity();

	//アンカーポイントを中点に
	anchorPoint_ = XMFLOAT2(0.5f, 0.5f);
}

void Sprite::Update()
{

	float left = (0.0f - anchorPoint_.x) * scale_.x;
	float right = (1.0f - anchorPoint_.x) * scale_.x;
	float top = (0.0f - anchorPoint_.y) * scale_.y;
	float bottom = (1.0f - anchorPoint_.y) * scale_.y;

	//頂点データ
	vertices_[0] = { {   left,bottom , 0.0f },{ 0.0f,1.0f } };	//左下
	vertices_[1] = { {  left,  top, 0.0f },{ 0.0f,0.0f } };	//左上
	vertices_[2] = { { right,bottom , 0.0f },{ 1.0f,1.0f } };	//右下
	vertices_[3] = { { right,bottom , 0.0f },{ 1.0f,1.0f } };	//右下
	vertices_[4] = { {   left,  top, 0.0f },{ 0.0f,0.0f } };	//左上
	vertices_[5] = { { right,  top, 0.0f },{ 1.0f,0.0f } };	//右上

	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//マテリアル
	constMapMaterial_->color = color;

	//変形行列
	//ワールド変換行列
	XMMATRIX matWorld_;
	matWorld_ = DirectX::XMMatrixIdentity();
	//回転
	XMMATRIX matRot;
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(rotation_);
	matWorld_ *= matRot;
	//平行移動
	XMMATRIX matTrans;
	matTrans = DirectX::XMMatrixTranslation(position_.x, position_.y, 0.0f);
	matWorld_ *= matTrans;

	//2D座標に変換(ビューポート変換)
	matWorld_ = DirectX::XMMatrixIdentity();
	matWorld_.r[0].m128_f32[0] = 2.0f / window_width;
	matWorld_.r[1].m128_f32[1] = -2.0f / window_height;
	matWorld_.r[3].m128_f32[0] = -1.0f + (position_.x / window_width) * 2;
	matWorld_.r[3].m128_f32[1] = 1.0f - (position_.y / window_height) * 2;

	//代入
	constMapTransform_->mat = matWorld_;

}

void Sprite::Draw(ID3D12GraphicsCommandList* cmdList_)
{
	//パイプライン、ルートシグネチャをセット
	cmdList_->SetPipelineState(pipelinestate_.Get());
	cmdList_->SetGraphicsRootSignature(rootsignature_.Get());
	//プリミティブ形状の設定コマンド
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	//定数バッファビューの設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { spriteManager_->GetSrvHeap() };
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = spriteManager_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
	//ハンドル1分のサイズ
	UINT incrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//テクスチャの番号に合わせてハンドルを進める
	if (textureNum_ > 0)
	{
		srvGpuHandle.ptr += incrementSize * textureNum_;
	}

	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	//定数バッファビューの設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//描画コマンド
	cmdList_->DrawInstanced(_countof(vertices_), 1, 0, 0);
}

void Sprite::CreateGraphicsPipeLine()
{
	HRESULT result;

	ID3DBlob* vsBlob = nullptr;	//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;	//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;	//エラーオブジェクト

	//頂点シェーダの読み込み コンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpriteVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	//エラーの場合
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//ピクセルシェーダの読み込み コンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpritePixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	//エラーの場合
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//頂点レイアウト
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

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定

	//ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//深度クリッピングを有効に

	//ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//共通の設定
	pipelineDesc.BlendState.RenderTarget[0].BlendEnable = true;
	pipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	pipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//アルファブレンド
	pipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他設定
	pipelineDesc.NumRenderTargets = 1;	//描画対象は1
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//RGBA
	pipelineDesc.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング

	//テクスチャサンプラーの設定
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

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParameter[3] = {};
	//定数バッファ0番
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].Descriptor.ShaderRegister = 0;
	rootParameter[0].Descriptor.RegisterSpace = 0;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//テクスチャレジスタ0番
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorRange;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//定数バッファ1番
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[2].Descriptor.ShaderRegister = 1;
	rootParameter[2].Descriptor.RegisterSpace = 0;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParameter;
	rootSignatureDesc.NumParameters = _countof(rootParameter);
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootsignature_.Get();

	//パイプラインステートの生成
	result = device_->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelinestate_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}
