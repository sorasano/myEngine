/**
* @file Shadow.h
* @brief 影
*/

#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"

class Shadow
{
public:

	//コンストラクタ
	Shadow();

	/**
	* 初期化
	*
	* @param[in] model 影モデル
	*/
	void Initialize(FbxModel* model);
	/**
	* 更新
	*
	* @param[in] pos 実体座標
	* @param[in] rot 回転情報
	*/
	void Update(const XMFLOAT3 pos, const XMFLOAT3 rot);
	/**
	* 行列更新
	*/
	void UpdateMatrix();
	/**
	* 描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/**
	* モデルセット
	*/
	void SetModel(FbxModel* EnemyModel);

private:

	//fbx
	std::unique_ptr<FbxObject3D> shadowObject_ = nullptr;

	//アフィン変換情報
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,0.1f };

	//床
	float groundPos_ = -11.5f;
};

