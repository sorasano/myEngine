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
	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

public:	//静的メンバ変数セッター
	static void SetDevice(ID3D12Device* device) { Sprite::device = device; }
	static void SetSpriteManager(SpriteManager* spriteManager) { Sprite::spriteManager = spriteManager; }
	static void CreateGraphicsPipeLine();

public:	//セッター
	//アルファ値
	void SetAlpha(float alpha) { color.w = alpha; }
	//色
	void SetColor(XMFLOAT3 c) { color.x = c.x; color.y = c.y; color.z = c.z; }
	//テクスチャの番号をセット
	void SetTextureNum(int num) { textureNum = num; }
	//座標
	void SetPosition(XMFLOAT2 pos) { position = pos, InitPosition = pos; }
	//角度
	void SetRotation(float rot) { rotation = rot; }
	//スケール
	void SetScale(XMFLOAT2 sca) { scale = sca; }

	void SetAnchorPoint(const DirectX::XMFLOAT2& point) { anchorPoint = point; }

public:	//ゲッター
	//座標
	XMFLOAT2 GetPosition() { return position; }
	//角度
	float GetRotation() { return rotation; }
	//スケール
	XMFLOAT2 GetScale() { return scale; }

private:
	//デバイス
	static ID3D12Device* device;
	//スプライトマネージャー
	static SpriteManager* spriteManager;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootsignature;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelinestate;

private:	//メンバ変数
	//使用するテクスチャの番号
	int textureNum = 0;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	//頂点データ
	Vertex vertices[6];
	Vertex* vertMap = nullptr;
	//定数バッファ マテリアル
	ComPtr<ID3D12Resource>constBuffMaterial;
	ConstBuffMaterial* constMapMaterial = nullptr;
	//定数バッファ 変形行列
	ComPtr<ID3D12Resource>constBuffTransform;
	ConstBuffTransform* constMapTransform = nullptr;

	//アンカーポイント(座標変換の基準点)
	DirectX::XMFLOAT2 anchorPoint = { 0.0f,0.0f };

public:

	//テクスチャの色
	XMFLOAT4 color = { 1,1,1,1 };

	//---演出用---
	//画面外にでる演出用
	Easing flipOutEase;
	//フリップ初期化
	bool initFlip = false;
	//演出中か
	bool isflipEase = false;
	//演出の段階
	int flipInFase;
	//揺れ幅
	float flipInRangeUp = 50;
	float flipInRangeDown = 100;
	//演出が終わったか
	bool endFlip = false;

	//揺れ用
	Easing swayEase;
	//揺れ初期化
	bool initSway = false;
	//揺れ幅
	float swayRange = 100;
	//揺れてるか
	bool isSway = false;
	//上昇中か下降中か
	bool isSwayUp = true;
	//演出開始位置
	DirectX::XMFLOAT2 startEasePos;
	//中心点
	DirectX::XMFLOAT2 swayCenterPos;

public:

	//演出

	//画面外へアウト
	void StartFlipOut() { isflipEase = true; };
	void FlipOut();

	//揺れる	center=中心点
	void StartSway(DirectX::XMFLOAT2 center) { isSway = true, swayCenterPos = center; }
	void Sway();

	//リセット
	void Reset();

private:
	float rotation = 0;
	XMFLOAT2 position = { 0,0 };
	XMFLOAT2 scale = { 100.0f,100.0f };

	XMFLOAT2 InitPosition = { 0,0 };

};

