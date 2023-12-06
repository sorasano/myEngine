/**
* @file object3D.h
* @brief objの処理
*/

#pragma once
#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "assert.h"
#include <DirectXTex.h>

#include "wrl.h"
#include "Model.h"
#include "Base/DirectXCommon.h"

using namespace DirectX;
using namespace Microsoft::WRL;

////頂点データ構造体
//struct Vertex
//{
//	XMFLOAT3 pos;	//座標
//	XMFLOAT3 normalize;	//法線ベクトル
//	XMFLOAT2 uv;	//uv座標
//	Vertex* parent = nullptr;
//};
//
//struct Indices
//{
//	int num;
//};

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//色(RGBA)
};

//定数バッファ用データ構造体（3D変換行列）
struct ConstBufferDataTransform
{
	XMMATRIX mat;
};

//3Dオブジェクト型
struct Object3d2
{
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファマップ
	ConstBufferDataTransform* constMapTransform_;
	//アフィン変換情報
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld_;
	//親オブジェクトのポインター
	Object3d2* parent = nullptr;
};

//3オブジェクト関連
/**
* 3Dオブジェクト初期化
*
* @param[in] object 説明
* @param[out] device 説明
*/
void InitializeObject3d(Object3d2* object, ComPtr<ID3D12Device> device);
/**
* 3Dオブジェクト更新
*
* @param[in] paramA 説明
* @param[out] paramB 説明
*/
void UpdateObject3d(Object3d2* object, XMMATRIX& matView, XMMATRIX& matProjection);
/**
* 3Dオブジェクト描画処理
*
* @param[in] paramA 説明
* @param[out] paramB 説明
*/
void DrawObject3d(Object3d2* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

class Object3D
{
public:
	/**
	* シングルトンインスタンス
	*/
	Object3D* GetInstance();
	/**
	* コンストラクタ
	*/
	Object3D();
	/**
	* デストラクタ
	*/
	~Object3D();
	/**
	* 初期化
	*
	* @param[in] dx dxCommon
	* @param[in] model objモデル
	*/
	void Initialize(DirectXCommon* dx,Model* model);
	/**
	* 更新
	*
	* @param[in] matView ビュー行列
	* @param[in] matProjection プロジェクヨン行列
	*/
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	/**
	* 描画
	*
	* @param[in] vbView 頂点バッファ
	* @param[in] ibView インデックスバッファ
	*/
	void Draw(D3D12_VERTEX_BUFFER_VIEW& vbView,D3D12_INDEX_BUFFER_VIEW& ibView);
	//ゲッター　セッター　
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition() const { return position_; };
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotation() const { return rotation_; };
	/**
	* @return XMFLOAT3 scale_取得
	*/
	XMFLOAT3 GetScale() const { return scale_; };
	/**
	* posセット
	*/
	void setPosition(XMFLOAT3 pos);
	/**
	* rotセット
	*/
	void setRotation(XMFLOAT3 rot);
	/**
	* scaセット
	*/
	void setScale(XMFLOAT3 sca);
public:
	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		XMMATRIX mat;	// ３Ｄ変換行列
	};
	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;	//アンビエント係数
		float pad1;			//パディング
		XMFLOAT3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		XMFLOAT3 specular;	//スペキュラー係数
		float alpha;		//アルファ
	};
private:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファマップ
	ConstBufferDataTransform* constMapTransform_;
	//モデル
	Model* model_ = nullptr;
	//デバイス
	DirectXCommon* dx_ = nullptr;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0; 
	ComPtr<ID3D12Resource> constBuffB1;
	// コマンドリスト
	ID3D12GraphicsCommandList* cmdList_;
private:
	//アフィン変換情報
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0};
	XMFLOAT3 position_ = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld_;
};