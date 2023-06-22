#pragma once

#include "fbxsdk.h"
#include "FbxModel.h"

#include <d3d12.h>
#include <d3dx12.h>

#include <string>

class FbxLoader
{

private://エイリアス
	using string = std::string;	//std::を省略

public:
	//モデル格納ルートパス
	static const string baseDirectory;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	void Initialize(ID3D12Device* device);

	void Finalize();

public:
	//FBXファイルの読み込み
	FbxModel* LoadModelFromFile(const string& modelName);
	//再起敵にノード構成を解析
	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);
	//メッシュ読み取り
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);

	/// <summary>
	/// FBXの行列をXMatrixiに変換
	/// </summary>
	/// <param name="dst">書き込み先</param>
	/// <param name="src">元となるFBX行列</param>
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

public://メッシュサブ関数
	//頂点座標読み取り
	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	//マテリアル読み取り
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	//テクスチャ読み込み
	void LoadTexture(FbxModel* model, const std::string& fullpath);
	//スキニング情報の読み取り
	void ParseSkin(FbxModel* model,FbxMesh* fbxMesh);

private:
	//D3D12デバイス
	ID3D12Device* device = nullptr;
	//FBXマネージャー
	FbxManager* fbxManager = nullptr;
	//FBXインポーター
	FbxImporter* fbxImporter = nullptr;

private:
	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

};