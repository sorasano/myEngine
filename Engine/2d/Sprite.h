/**
* @file Sprite.h
* @brief スプライトの処理
*/

#pragma once
#include "d3dx12.h"
#include "DirectXMath.h"
#include "array"
#include "SpriteManager.h"
#include "Easing.h"

class Sprite
{
private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

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
	*	初期化
	*/
	void Initialize();
	/**
	*	更新
	*/
	void Update();
	/**
	* 描画
	*
	* @param[out] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList_);

public:
	//静的メンバ変数セッター
	/**
	* deviceセット
	*/
	static void SetDevice(ID3D12Device* device) { Sprite::device_ = device; }
	/**
	* spriteManagerセット
	*/
	static void SetSpriteManager(SpriteManager* spriteManager) { Sprite::spriteManager_ = spriteManager; }
	/**
	* パイプライン生成
	*/
	static void CreateGraphicsPipeLine();

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
	* テクスチャの番号セット
	*/
	void SetTextureNum(int num) { textureNum_ = num; }
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
	/**
	* アンカーポイントセット
	*/
	void SetAnchorPoint(const DirectX::XMFLOAT2& point) { anchorPoint_ = point; }

public:	//ゲッター
	/**
	* @return XMFLOAT2 座標取得
	*/
	XMFLOAT2 GetPosition() { return position_; }
	/**
	* @return float 角度取得
	*/
	float GetRotation() { return rotation_; }
	/**
	* @return XMFLOAT2 スケール取得
	*/
	XMFLOAT2 GetScale() { return scale_; }
	/**
	* @return XMMATRIX ワールド変換行列取得
	*/
	XMMATRIX GetMatWorld() { return viewPort_; }


private:
	//デバイス
	static ID3D12Device* device_;
	//スプライトマネージャー
	static SpriteManager* spriteManager_;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootsignature_;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelinestate_;

private:	//メンバ変数
	//使用するテクスチャの番号
	int textureNum_ = 0;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//頂点データ
	Vertex vertices_[6] = {};
	Vertex* vertMap_ = nullptr;
	//定数バッファ マテリアル
	ComPtr<ID3D12Resource>constBuffMaterial_;
	ConstBuffMaterial* constMapMaterial_ = nullptr;
	//定数バッファ 変形行列
	ComPtr<ID3D12Resource>constBuffTransform_;
	ConstBuffTransform* constMapTransform_ = nullptr;

public:

	//テクスチャの色
	XMFLOAT4 color = { 1,1,1,1 };

private:
	XMMATRIX viewPort_;
	float rotation_ = 0;
	XMFLOAT2 position_ = { 0,0 };
	XMFLOAT2 scale_ = { 100.0f,100.0f };

	//アンカーポイント(座標変換の基準点)
	DirectX::XMFLOAT2 anchorPoint_ = { 0.0f,0.0f };
};

