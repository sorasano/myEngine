/**
* @file Skydome.cpp
* @brief 天球の処理
*/

#pragma once
#include "DirectXCommon.h"
#include "FbxLoader.h"
#include "FbxObject3D.h"

class Skydome
{
public:
	/**
	* 初期化
	*/
	void Initialize();
	/**
	* 更新処理
	* @param[in] eyeZ カメラの座標
	*/
	void Update(float eyeZ);
	/**
	* 描画
	* @param[out] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList_);

	/**
	* スカイドームの端の取得(スケール/2)
	*
	* @return float スカイドームの端
	*/
	float GetEdge() const { return scale_.z / 2; }

private:
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1000,1000,1000 };

	std::unique_ptr<FbxModel> skydomeModel = nullptr;
	std::unique_ptr<FbxObject3D> skydomeObject = nullptr;
};

