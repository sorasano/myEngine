/**
* @file ParticleManager.cpp
* @brief パーティクルの処理
*/

#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
//const float ParticleManager::radius = 5.0f;				// 底面の半径
//const float ParticleManager::prizmHeight = 8.0f;			// 柱の高さ
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
MyEngine::DirectXCommon* ParticleManager::dx_ = nullptr;

/// 静的メンバ変数の実態
const std::string ParticleManager::baseDirectory = "Resources/effect/";

//XMFLOAt3同士の加算処理
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

void ParticleManager::StaticInitialize(MyEngine::DirectXCommon* dx)
{
	// nullptrチェック
	assert(dx);

	ParticleManager::dx_ = dx;
	ParticleManager::device_ = dx->GetDevice();

	// カメラ初期化
	InitializeCamera(window_width, window_height);

	// パイプライン初期化
	InitializeGraphicsPipeline();

	// テクスチャ読み込み
	//LoadTexture();

	//// モデル生成
	//CreateModel();

}

void ParticleManager::StaticUpdate(XMFLOAT3 eye, XMFLOAT3 target)
{
	ParticleManager::eye_ = eye;
	ParticleManager::target_ = target;
}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(ParticleManager::cmdList_ == nullptr);

	// コマンドリストをセット
	ParticleManager::cmdList_ = cmdList;

	// パイプラインステートの設定
	cmdList_->SetPipelineState(pipelinestate_.Get());
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(rootsignature_.Get());
	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleManager::PostDraw()
{
	// コマンドリストを解除
	ParticleManager::cmdList_ = nullptr;
}

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

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = 1; // シェーダーリソースビュー1つ
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

void ParticleManager::InitializeCamera(int windowWidth, int windowHeight)
{
	//// ビュー行列の生成
	//matView = XMMatrixLookAtLH(
	//	XMLoadFloat3(&eye),
	//	XMLoadFloat3(&target),
	//	XMLoadFloat3(&up));
	UpdateViewMatrix();


	// 平行投影による射影行列の生成
	//constMap->mat = XMMatrixOrthographicOffCenterLH(
	//	0, window_width,
	//	window_height, 0,
	//	0, 1);
	// 透視投影による射影行列の生成
	matProjection_ = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		(float)windowWidth / windowHeight,
		0.1f, 1000.0f
	);
}

void ParticleManager::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;	// ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticleVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
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

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticlePS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
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


	// ジオメトリシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ParticleGS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&gsBlob, &errorBlob);
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

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//デプスの書き込み禁止
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
	assert(SUCCEEDED(result));

}

void ParticleManager::LoadTexture(const std::string& resourcename)
{

	HRESULT result = S_FALSE;

	TexMetadata metadata_{};
	ScratchImage scratchImg_{};

	//連結してフルパスを得る
	const std::string fullpath = baseDirectory + resourcename;

	//ユニコード文字列に変換する
	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0,
	fullpath.c_str(), -1, wfilepath, _countof(wfilepath));

	// WICテクスチャのロード
	result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metadata_, scratchImg_);

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg_.GetImages(), scratchImg_.GetImageCount(), scratchImg_.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg_ = std::move(mipChain);
		metadata_ = scratchImg_.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata_.format = MakeSRGB(metadata_.format);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format, metadata_.width, (UINT)metadata_.height, (UINT16)metadata_.arraySize,
		(UINT16)metadata_.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr, IID_PPV_ARGS(&texbuff_));
	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata_.mipLevels; i++) {
		const Image* img = scratchImg_.GetImage(i, 0, 0); // 生データ抽出
		result = texbuff_->WriteToSubresource(
			(UINT)i,
			nullptr,              // 全領域へコピー
			img->pixels,          // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	// シェーダリソースビュー作成
	cpuDescHandleSRV_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);
	gpuDescHandleSRV_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap_->GetGPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff_->GetDesc();
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device_->CreateShaderResourceView(texbuff_.Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
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

	//四角形のインデックスデータ
	unsigned short indicesSquare[] = {
		0,1,2,//三角形1
		2,1,3,//三角形2
	};

	////メンバ変数にコピー
	//std::copy(std::begin(indicesSquare), std::end(indicesSquare),indices);

	UINT sizeVB = static_cast<UINT>(sizeof(vertices_));

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPos* vertMap_ = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	if (SUCCEEDED(result)) {
		memcpy(vertMap_, vertices_, sizeof(vertices_));
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(vertices_);
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	//UINT sizeIB = static_cast<UINT>(sizeof(indices));
	//// リソース設定
	//resourceDesc.Width = sizeIB;

	//// インデックスバッファ生成
	//result = device->CreateCommittedResource(
	//	&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	//	IID_PPV_ARGS(&indexBuff));

	//// インデックスバッファへのデータ転送
	//unsigned short* indexMap = nullptr;
	//result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//if (SUCCEEDED(result)) {

	//	// 全インデックスに対して
	//	for (int i = 0; i < _countof(indices); i++)
	//	{
	//		indexMap[i] = indices[i];	// インデックスをコピー
	//	}

	//	indexBuff->Unmap(0, nullptr);
	//}

	//// インデックスバッファビューの作成
	//ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//ibView.Format = DXGI_FORMAT_R16_UINT;
	//ibView.SizeInBytes = sizeof(indices);
}

void ParticleManager::UpdateViewMatrix()
{
	//// ビュー行列の更新
	//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMVECTOR eyePosition = XMLoadFloat3(&eye_);
	XMVECTOR targetPosition = XMLoadFloat3(&target_);
	XMVECTOR upVector = XMLoadFloat3(&up_);

	//カメラZ軸
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	//0ベクトルを除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	//ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//カメラX軸
	XMVECTOR cameraAxisX;
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	//カメラY軸
	XMVECTOR cameraAxisY;
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//ベクトルを正規化
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	//カメラ回転行列
	XMMATRIX matCameraRot;
	//カメラ座標系からワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	//転置により逆行列を計算
	matView_ = XMMatrixTranspose(matCameraRot);

	//視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//カメラの位置からワールド原点へのベクトル
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);

	//1つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//ビュー変換行列に平行移動成分を設定
	matView_.r[3] = translation;

#pragma region 全方向ビルボード行列の計算
	//ビルボード行列
	matBillbord_.r[0] = cameraAxisX;
	matBillbord_.r[1] = cameraAxisY;
	matBillbord_.r[2] = cameraAxisZ;
	matBillbord_.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma region

#pragma region Y軸回りビルボード行列の計算

	//カメラX軸,Y軸,Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X軸は共通
	ybillCameraAxisX = cameraAxisX;
	//Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z軸はX軸→Y軸の外積
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	//Y軸回りビルボード行列
	matBillbordY_.r[0] = ybillCameraAxisX;
	matBillbordY_.r[1] = ybillCameraAxisY;
	matBillbordY_.r[2] = ybillCameraAxisZ;
	matBillbord_.r[3] = XMVectorSet(0, 0, 0, 1);

#pragma region

}

bool ParticleManager::Initialize(const std::string& resourcename)
{

	// デスクリプタヒープの初期化
	InitializeDescriptorHeap();

	LoadTexture(resourcename);

	// モデル生成
	CreateModel();

	// nullptrチェック
	assert(device_);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	HRESULT result;

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	return true;
}

void ParticleManager::Update(XMFLOAT3 cameraPos)
{

	UpdateViewMatrix();

	//寿命が尽きたパーティクルを削除
	particles_.remove_if([](Particle& x) {return x.frame >= x.num_flame; });

	//死亡フラグの立ったパーティクルを削除
	particles_.remove_if([](Particle& x) {return x.isDead; });

	//全パーティクル更新
	for (std::forward_list<Particle>::iterator it = particles_.begin(); it != particles_.end(); it++) {

		//経過フレーム数をカウント
		it->frame++;
		//速度に加速度を加算
		it->velocity = it->velocity + it->accel;
		//速度による移動
		it->position_ = it->position_ + it->velocity;

		//カメラの近くに来たパーティクルを削除
		if (it->position_.z <= cameraPos.z + 10) {
			it->isDead = true;
		}
	}

	BuffUpdate();
}

void ParticleManager::Draw()
{
	// nullptrチェック
	assert(device_);
	//assert(ParticleManager::cmdList);

	this->cmdList_ = dx_->GetCommandList();

	// パイプラインステートの設定
	cmdList_->SetPipelineState(pipelinestate_.Get());
	//// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(rootsignature_.Get());
	//// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView_);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	cmdList_->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV_);
	// 描画コマンド
	cmdList_->DrawInstanced((UINT)std::distance(particles_.begin(), particles_.end()), 1, 0, 0);

}

void ParticleManager::Add(int life, const XMFLOAT3& position, const XMFLOAT3& velocity, const XMFLOAT3& accel)
{
	//リストに要素を追加
	particles_.emplace_front();
	//追加した要素の参照
	Particle& p = particles_.front();
	//値のセット
	p.position_ = position;
	p.velocity = velocity;
	p.accel = accel;
	p.num_flame = life;
}

void ParticleManager::MakeParticle(const XMFLOAT3& position)
{
	for (int i = 0; i < 100; i++) {
		//X,Y,Zすべてpositionから[+1.0f,-1.0f]でランダムに分布

		XMFLOAT3 pos{};
		pos.x = Random(position.x - 1.0f, position.x + 1.0f);
		pos.y = Random(position.y - 1.0f, position.y + 1.0f);
		pos.z = Random(position.z - 1.0f, position.z + 1.0f);

		//X,Y,Zすべて[-0.05f,+0.05f]でランダムに分布
		const float md_vel = 0.05f;
		XMFLOAT3 vel{};
		vel.x = Random(-md_vel, md_vel);
		vel.y = Random(-md_vel, md_vel);
		vel.z = Random(-md_vel, md_vel);

		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		XMFLOAT3 acc{};
		const float md_acc = -0.001f;
		acc.y = Random(md_acc, 0);

		//追加
		Add(enemyDestroyParticleTime_, pos, vel, acc);

	}
}

void ParticleManager::BuffUpdate()
{
	HRESULT result;

	//頂点バッファへデータ転送
	VertexPos* vertMap_ = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	if (SUCCEEDED(result)) {
		//パーティクルの情報を1つずつ反映
		for (std::forward_list<Particle>::iterator it = particles_.begin(); it != particles_.end(); it++) {
			//座標
			vertMap_->pos = it->position_;
			//次の頂点へ
			vertMap_++;
		}
		vertBuff_->Unmap(0, nullptr);
	}


	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matView_ * matProjection_;	// 行列の合成
	constMap->matbBillboard = matBillbord_;
	constBuff_->Unmap(0, nullptr);
}


