/**
* @file FbxObject3d.h
* @brief Fbx処理
*/

#pragma once
#include "FbxModel.h"
#include "Camera.h"

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string.h>

class FbxObject3D
{
public: //定数
	//ボーンの最大数
	static const int MAX_BONES = 32;
public:
	//サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferDataTransform
	{
		XMMATRIX world;
	};

	//定数バッファ用データ構造体
	struct ConstBufferDataCamera
	{
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};

	//定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin {
		XMMATRIX bones[MAX_BONES];
	};

private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:	//静的メンバ関数
	//セッター

	/**
	* デバイスのセット
	*/
	static void SetDevice(ID3D12Device* device) { FbxObject3D::device_ = device; }
	/**
	* カメラのセット
	*/
	static void SetCamera(Camera* camera) { FbxObject3D::camera_ = camera; }

	/**
	* グラフィックパイプラインの生成
	*/
	static void CreateGraphicsPipeline();

private://静的メンバ変数
	static ID3D12Device* device_;
	static Camera* camera_;

	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature_;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate_;

public://メンバ関数
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
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/**
	* モデルのセット
	*/
	void SetModel(FbxModel* model) { this->model_ = model; }

	/**
	* アニメーション開始
	*/
	void PlayAnimation();

	//セッター
	/**
	* positionセット
	*/
	void SetPosition(XMFLOAT3 position) { this->position_ = position; }
	/**
	* rotateセット
	*/
	void SetRotate(XMFLOAT3 rotate) { this->rotation_ = rotate; }
	/**
	* scaleセット
	*/
	void SetScale(XMFLOAT3 scale) { this->scale_ = scale; }

	//ゲッター
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition() { return this->position_; }
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotate() { return this->rotation_ ; }
	/**
	* @return XMFLOAT3 scale_取得
	*/
	XMFLOAT3 GetScale() { return this->scale_ ; }

protected://メンバ変数
	//定数バッファ(ワールド座標)
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBuffSkin_;

private:

	//ローカルスケール
	XMFLOAT3 scale_ = { 1,1,1 };
	//X,Y,Z軸回りのローカル行列
	XMFLOAT3 rotation_ = { 0,0,0 };
	//ローカル座標
	XMFLOAT3 position_ = { 0,0,0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld_ = {};
	//モデル
	FbxModel* model_ = nullptr;

	//1フレームの時間
	FbxTime frameTime_;
	//アニメーション開始時間
	FbxTime startTime_;
	//アニメーション終了時間
	FbxTime endTime_;
	//現在時間
	FbxTime currentTime_;
	//アニメーション再生中
	bool isPlay_ = false;
};

