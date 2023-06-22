#pragma once


#include<DirectXMath.h>
#include"Texture.h"
#include<string>
#include"Vector3.h"
#include"SpriteManager.h"
#include"Easing.h"

enum VertexNumber {
	LB,	//左下
	LT,	//左上
	RB,	//右下
	RT,	//右上
};

class Sprite
{

private:


public:	//メンバ変数

	//マネージャクラスのポインタ
	SpriteManager* spriteManager = nullptr;

	VertexPosUv vertices[4] = {};		//頂点座標データ
	D3D12_VERTEX_BUFFER_VIEW vbView{};	//頂点バッファビュー
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff = nullptr;	//定数バッファ
	uint32_t textureIndex = 0;	//テクスチャに割り当てられている番号
	ConstBufferData* constMap = nullptr;//定数バッファ構造体

	DirectX::XMMATRIX matWorld{};	//ワールド変換行列
	float rotation = 0.0f;	//回転角
	DirectX::XMFLOAT2 position = { 0.0f,0.0f };	//座標
	DirectX::XMFLOAT4 color = { 1,1,1,1 };	//座標
	DirectX::XMFLOAT2 size = { 100.0f,100.0f };//スプライトサイズ
	DirectX::XMFLOAT2 anchorPoint = { 0.0f,0.0f };//アンカーポイント(座標変換の基準点)
	bool isFlipX = false;	//左右反転フラグ
	bool isFlipY = false;	//上下反転フラグ
	bool isInvisible = false;	//非表示フラグ

	DirectX::XMFLOAT2 textureLeftTop = { 0.0f,0.0f };	//テクスチャ左上座標
	DirectX::XMFLOAT2 textureSize = { 100.0f,100.0f };	//テクスチャ切り出しサイズ

	//画面外から入ってくる演出用
	Easing flipInEase;
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


public: //メンバ関数
	void Initialize(uint32_t textureNum = UINT32_MAX);

	void Draw();
	void SetColor(const DirectX::XMFLOAT4& color_) { color = color_; }
	void SetPos(const DirectX::XMFLOAT2& pos) { position = pos; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetSize(const DirectX::XMFLOAT2& size_) { size = size_; }
	void SetAnchorPoint(const DirectX::XMFLOAT2& point) { anchorPoint = point; }
	void SetFlipX(bool flipX) { isFlipX = flipX; }
	void SetFlipY(bool flipY) { isFlipY = flipY; }
	void SetInvisible(bool flag) { isInvisible = flag; }
	void SetTextureNum(uint32_t index) { textureIndex = index; }
	void SetTextureLeftTop(const DirectX::XMFLOAT2& leftTop) { textureLeftTop = leftTop; }
	void SetTextureSize(const DirectX::XMFLOAT2& size) { textureSize = size; }

	const DirectX::XMFLOAT2& GetPosition()const { return position; }
	float GetRotation()const { return rotation; }
	const DirectX::XMFLOAT4 GetColor()const { return color; }
	const DirectX::XMFLOAT2 GetSize()const { return size; }
	const DirectX::XMFLOAT2 GetAnchorPoint()const { return anchorPoint; }
	bool GetIsFlipX()const { return isFlipX; }
	bool GetIsFlipY()const { return isFlipY; }
	bool GetIsInvisible()const { return isInvisible; }
	uint32_t GetTextureNum()const { return textureIndex; }
	const DirectX::XMFLOAT2 GetTextureLeftTop()const { return textureLeftTop; }
	const DirectX::XMFLOAT2 GetTextureSize()const { return textureSize; }

	void Update();

	//演出

	//画面外へアウト
	void StartFlipOut() { isflipEase = true; };
	void FlipOut();

	//揺れる	center=中心点
	void StartSway(DirectX::XMFLOAT2 center) { isSway = true, swayCenterPos = center; }
	void Sway();

private:
	//テクスチャサイズをイメージサイズに合わせる
	void AdjustTextureSize();
};

