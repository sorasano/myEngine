#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "DirectXCommon.h"

using namespace Microsoft::WRL;
using namespace DirectX;

class Model
{
public:
	// 頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT4 color;
		XMFLOAT2 uv;  // uv座標
	};

	//// 定数バッファ用データ構造体B0
	//struct ConstBufferDataB0
	//{
	//	XMMATRIX mat;	// ３Ｄ変換行列
	//};
	//// 定数バッファ用データ構造体B1
	//struct ConstBufferDataB1
	//{
	//	XMFLOAT3 ambient;	//アンビエント係数
	//	float pad1;			//パディング
	//	XMFLOAT3 diffuse;	//ディフューズ係数
	//	float pad2;			//パディング
	//	XMFLOAT3 specular;	//スペキュラー係数
	//	float alpha;		//アルファ
	//};
	//マテリアル
	struct Material
	{
		std::string name;	//マテリアル名
		XMFLOAT3 ambient;	//アンビエント影響度
		XMFLOAT3 diffuse;	//ディフューズ影響度
		XMFLOAT3 specular;	//スペキュラー影響度
		float alpha;	//アルファ
		std::string textureFilename;	//テクスチャファイル名
		//コンストラクタ
		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};
public:
	//シングルトンインスタンス
	Model *GetInstance();
	//初期化			dxCommon,			objとmtlが入ってるファイルの名前,	貼り付ける画像の名前
	void Initialize(DirectXCommon* dx_, const std::string& filename , const std::string& resourcename);

	void InitializeVertex(const std::string& filename);	//頂点初期化
	void InitializeDesc();	//デスクリプタ初期化
	void CompileShader(const wchar_t* file, const wchar_t* file2);	//シェーダ読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	void LoadTexture(const std::string& resourcename);
	//更新
	void Update();
	//ゲッター
	Material GetMaterial() { return material; };
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return descHeap_; };
	ComPtr<ID3D12PipelineState> GetPipelinestate() { return pipelinestate_; };
	ComPtr<ID3D12RootSignature> GetRootSignature() { return rootsignature_; };
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv() { return gpuDescHandleSRV_; };
	UINT GetIndicesSize() { return (UINT)sizeof(indices.size()); };
public:
	DirectXCommon* dx_ = nullptr;
	ComPtr<ID3D12Resource> constBuffMaterial_;
public:
	D3D12_RECT scissorRect{};
	D3D12_VIEWPORT viewport{};

	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};		//頂点バッファビュー
	uint32_t textureIndex = 0;	//テクスチャ番号

private:
	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;	//座標
		XMFLOAT3 normalize;	//法線ベクトル
		XMFLOAT2 uv;	//uv座標
		XMFLOAT4 color = {1,0,0,1};
		Vertex* parent = nullptr;
	};

	struct Indices
	{
		int num;
	};
	//Vertex関連
	size_t verticesCount = 24;
	std::vector<VertexPosNormalUvSkin> vertices_;	//外部に渡す用の頂点データ
	size_t indicesCount = 36;
	std::vector<unsigned short> indices;
	//インデックスバッファ関連
	ComPtr<ID3D12Resource> indexBuff;
	//頂点バッファ初期化
	ComPtr<ID3D12Resource> vertBuff_;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature_;
	//パイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};	//グラフィックスパイプライン
	//パイプラインステート
	ComPtr<ID3D12PipelineState> pipelinestate_;
	//マテリアル
	Material material;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize_ = 0;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
};

