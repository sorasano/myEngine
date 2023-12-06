/**
* @file FbxModel.h
* @brief Fbxモデル処理
*/

#pragma once

#include <string>
#include <vector>
#include <DirectXmath.h>
#include <DirectXTex.h>

#pragma warning(push)
#pragma warning(disable:26495)
#pragma warning(disable:26813)
#pragma warning(disable:26451)
#include <fbxsdk.h>
#pragma warning(pop)


#include "windows.h"
#include "wrl.h"
#include "d3d12.h"
#include "d3dx12.h"

struct Node {
	//ローカルスケール
	DirectX::XMVECTOR  scaling = { 1,1,1,0 };
	//ローカル回転角
	DirectX::XMVECTOR  rotation_ = { 0,0,0,0 };
	//ローカル移動
	DirectX::XMVECTOR  translation = { 0,0,0,1 };
	//ローカル変形行列
	DirectX::XMMATRIX  transform;
	//グローバル変形行列
	DirectX::XMMATRIX  globalTransform;
	//親ノード
	Node* parent = nullptr;

	//名前
	std::string name;
};

class FbxModel
{
public:
	//フレンドクラス
	friend class FbxLoader;

public: //定数
	//ボーンインデックスの最大数
	static const int MAX_BONE_INDICES = 4;

public://サブクラス

	//頂点データ構造
	struct VertexPosNormalUvSkin {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];//ボーン番号
		float boneWeight[MAX_BONE_INDICES];//ボーン重み
	};

	//ボーン構造体
	struct Bone {
		//名前
		std::string name;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//クラスター(FBX側のボーン情報)
		FbxCluster* fbxCluster;
		//コンストラクタ
		Bone(const std::string& name) {
			this->name = name;
		}

	};

private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScracthImage = DirectX::ScratchImage;
	//std::を省略
	using string = std::string;
	template<class T>using vector = std::vector<T>;

public:

	/**
	* コンストラクタ
	*/
	FbxModel();
	/**
	* デストラクタ
	*/
	~FbxModel();

public:

	/**
	* バッファ生成
	*
	* @param[in] device_ デバイス
	*/
	void CreateBuffers(ID3D12Device* device);

	/**
	* 描画
	*
	* @param[in] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//ゲッター
	
	/**
	* @return const XMMATRIX モデルの変形行列取得取得
	*/
	const XMMATRIX& GetModelTransform(){ return meshNode->globalTransform; }
	/**
	* @return std::vector<Bone> ボーン配列取得
	*/
	std::vector<Bone>& GetBones(){ return bones; }
	/**
	* @return FbxScene FBXシーン取得
	*/
	FbxScene* GetFbxScene(){ return fbxScene; }

private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//インデックスバッファ
	ComPtr<ID3D12Resource>indexBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource>texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;

private:
	//モデル名
	std::string name;
	//ノード配列
	std::vector<Node> nodes;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	std::vector<VertexPosNormalUvSkin> vertices_;
	//頂点インデックス配列
	std::vector<unsigned short> indices;

	//アンビエント係数
	DirectX::XMFLOAT3 ambient = { 1,1,1 };
	//ディフューズ係数
	DirectX::XMFLOAT3 diffuse = { 1,1,1 };
	//テクスチャメタデータ
	DirectX::TexMetadata metadata_ = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg_ = {};

	//ボーン配列
	std::vector<Bone> bones;

	//FBXシーン
	FbxScene *fbxScene = nullptr;
};

