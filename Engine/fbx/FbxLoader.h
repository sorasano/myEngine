/**
* @file FbxLoader.h
* @brief Fbxの読み込み
*/

#pragma once

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

	/**
	* シングルトンインスタンスの取得
	* 
	* @return FbxLoader インスタンス
	*/
	static FbxLoader* GetInstance();

	/**
	* 初期化
	*/
	void Initialize(ID3D12Device* device);

	/**
	* 終了
	*/
	void Finalize();

public:
	/**
	* FBXファイルの読み込み
	*
	* @param[in] modelName fbxモデルの名前
	* @return FbxModel fbxモデル
	*/
	FbxModel* LoadModelFromFile(const string& modelName);
	/**
	* 影モデルの読み込み
	*
	* @param[in] modelName fbxモデルの名前
	* @return FbxModel fbxモデル
	*/
	FbxModel* LoadShadowModelFromFile(const string& modelName);
	/**
	* 	再起敵にノード構成を解析
	*
	* @param[in,out] model モデル
	* @param[in] fbxNode FBXノード
	* @param[in] parent 親ノード
	*/
	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);
	/**
	* メッシュ読み取り
	*
	* @param[in] model モデル
	* @param[in] fbxNode FBXノード
	*/
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	/**
	* ディレクトリを含んだファイルパスからファイル名を抽出する
	*
	* @param[in,out] path ファイルパス
	* @return std::string ファイル名
	*/
	std::string ExtractFileName(const std::string& path);

	/**
	* FBXの行列をXMatrixiに変換
	*
	* @param[in] dst XMMATRIX型
	* @param[in] src 元となるFBX行列
	*/
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

public://メッシュサブ関数
	/**
	* 頂点座標読み取り
	*
	* @param[in] model モデル
	* @param[in] fbxMesh FBXメッシュ
	*/
	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	/**
	* 面情報読み取り
	*
	* @param[in] model モデル
	* @param[in] fbxMesh FBXメッシュ
	*/
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	/**
	* マテリアル読み取り
	*
	* @param[in] model モデル
	* @param[in] fbxNode FBXノード
	*/
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	/**
	* テクスチャ読み込み
	*
	* @param[in] model モデル
	* @param[in] fullpath フルパス
	*/
	void LoadTexture(FbxModel* model, const std::string& fullpath);
	/**
	* スキニング情報の読み取り
	*
	* @param[in] model モデル
	* @param[in] fbxMesh FBXメッシュ
	*/
	void ParseSkin(FbxModel* model,FbxMesh* fbxMesh);

private:
	//D3D12デバイス
	ID3D12Device* device_ = nullptr;
	//FBXマネージャー
	FbxManager* fbxManager_ = nullptr;
	//FBXインポーター
	FbxImporter* fbxImporter_ = nullptr;

private:
	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName_;
	//影用テクスチャ
	static const string shadowTextureFileName_;
	//影モデルか
	bool isShadow = false;

private:
	/**
	* privateなコンストラクタ（シングルトンパターン）
	*/
	FbxLoader() = default;
	/**
	* privateなデストラクタ（シングルトンパターン）
	*/
	~FbxLoader() = default;
	/**
	* コピーコンストラクタを禁止（シングルトンパターン）
	*/
	FbxLoader(const FbxLoader& obj) = delete;
	/**
	* コピー代入演算子を禁止（シングルトンパターン）
	*/
	void operator=(const FbxLoader& obj) = delete;

};