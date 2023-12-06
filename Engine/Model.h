/**
* @file Model.h
* @brief objモデルの処理
*/

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
	/**
	* @return Model インスタンス取得
	*/
	Model *GetInstance();

	/**
	* 初期化
	*
	* @param[in] dx_ dxCommon
	* @param[in] filename objとmtlが入ってるファイルの名前
	* @param[in] resourcename 貼り付ける画像の名前
	*/
	void Initialize(DirectXCommon* dx_, const std::string& filename , const std::string& resourcename);

	/**
	* 頂点初期化
	*
	* @param[in] modelname モデルの名前
	*/
	void InitializeVertex(const std::string& modelname);
	/**
	* デスクリプタ初期化
	*/
	void InitializeDesc();
	/**
	* シェーダ読み込み
	*
	* @param[in] file シェーダファイル名1
	* @param[in] file2 シェーダファイル名2
	*/
	void CompileShader(const wchar_t* file, const wchar_t* file2);
	/**
	* マテリアル読み込み
	*
	* @param[in] directoryPath ファイルパス
	* @param[out] filename マテリアルファイル名
	*/
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	/**
	* テクスチャ読み込み
	*
	* @param[in] resourcename テクスチャ名
	*/
	void LoadTexture(const std::string& resourcename);
	/**
	* 更新
	*/
	void Update();
	//ゲッター
	/**
	* @return int material取得
	*/
	Material GetMaterial() const { return material; };
	/**
	* @return int descHeap_取得
	*/
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() const { return descHeap_; };
	/**
	* @return int pipelinestate_取得
	*/
	ComPtr<ID3D12PipelineState> GetPipelinestate() const { return pipelinestate_; };
	/**
	* @return int rootsignature_取得
	*/
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootsignature_; };
	/**
	* @return int gpuDescHandleSRV_取得
	*/
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv() const { return gpuDescHandleSRV_; };
	/**
	* @return int indicesのサイズ取得
	*/
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

