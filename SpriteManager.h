#pragma once
#include<d3d12.h>
#include"DirectXCommon.h"
#include<DirectXMath.h>
#include"Texture.h"



struct VertexPosUv {
	DirectX::XMFLOAT3 pos;	//xyz座標
	DirectX::XMFLOAT2 uv;	//uv座標
};

struct ConstBufferData {
	DirectX::XMFLOAT4 color;
	DirectX::XMMATRIX mat;
};

class SpriteManager
{
private:
	SpriteManager();
	~SpriteManager();
	
public:
	//コピーコンストラクタ無効
	SpriteManager(const SpriteManager& obj) = delete;
	//代入演算子を無効
	SpriteManager& operator=(const SpriteManager& obj) = delete;

	//インスタンスアクセス専用関数
	static SpriteManager* GetInstance();
	
	static std::string defaultTextureDirectoryPath;
	DirectXCommon* directX = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;		//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;		//ルートシグネチャ

	DirectX::XMMATRIX matProjection{};//射影行列


public:
	//初期化
	void Initialize(DirectXCommon* directX,int windowWidth,int windowHeight);

	//描画前処理
	void beginDraw();

	/// <summary>
	/// テクスチャコマンドの発行
	/// </summary>
	/// <param name="dev"></param>
	void SetTextureCommand(uint32_t index);

	//指定番号のテクスチャバッファを取得
	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return Texture::texBuffuers[index].Get(); }

private:
	//スプライト用パイプラインステートとルートシグネチャの生成
	void CreatePipeline2D(ID3D12Device* dev);
};

