/**
* @file PostEffect.h
* @brief ポストエフェクトの処理
*/

#pragma once
#include "d3dx12.h"
#include "array"
#include "DirectXMath.h"
#include <DirectXTex.h>

#include "vector"

class PostEffect
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

public:	//サブクラス
	//定数バッファ
	struct ConstBuffMaterial
	{
		XMFLOAT4 color;
	};
	//定数バッファ2
	struct ConstBuffTransform
	{
		XMMATRIX mat;	//3D変換行列
	};
	//頂点データ用構造体
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

public:	//メンバ関数
	/**
	* 初期化
	*/
	void Initialize();
	/**
	* 更新
	*/
	void Update();
	/**
	* 描画
	*
	* @param[out] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList_);
	/**
	* パイプライン設定、作成
	*/
	void CreateGraphicsPipeLine();
	/**
	* 描画前処理
	*
	* @param[out] cmdList_ コマンドリスト
	*/
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList_);
	/**
	* 描画後処理
	*
	* @param[out] cmdList_ コマンドリスト
	*/
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList_);

public:	//静的メンバ関数
	/**
	* deviceセット
	*/
	static void SetDevice(ID3D12Device* device) { PostEffect::device_ = device; }

public:	//セッター
	/**
	* アルファ値セット
	*/
	void SetAlpha(float alpha) { color.w = alpha; }
	/**
	* 色セット
	*/
	void SetColor(XMFLOAT3 c) { color.x = c.x; color.y = c.y; color.z = c.z; }
	/**
	* 座標セット
	*/
	void SetPosition(XMFLOAT2 pos) { position_ = pos; }
	/**
	* 角度セット
	*/
	void SetRotation(float rot) { rotation_ = rot; }
	/**
	* スケールセット
	*/
	void SetScale(XMFLOAT2 sca) { scale_ = sca; }

private:	//静的メンバ変数
	//デバイス
	static ID3D12Device* device_;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootsignature_;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelinestate_;
	//画面クリアカラー
	static const float clearColor[4];

private:	//メンバ変数
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	//頂点データ
	Vertex vertices_[6];
	Vertex* vertMap_ = nullptr;
	//定数バッファ マテリアル
	ComPtr<ID3D12Resource>constBuffMaterial_;
	ConstBuffMaterial* constMapMaterial_ = nullptr;
	//定数バッファ 変形行列
	ComPtr<ID3D12Resource>constBuffTransform_;
	ConstBuffTransform* constMapTransform_ = nullptr;
	//テクスチャの色
	XMFLOAT4 color = { 1,1,1,1 };
	//テクスチャバッファ
	ComPtr<ID3D12Resource>textureBuff_;
	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;

	//深度バッファ
	ComPtr<ID3D12Resource>depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;

private:
	float rotation_ = 0;
	XMFLOAT2 position_ = { 0,0 };
	XMFLOAT2 scale_ = { 100.0f,100.0f };

};

