/**
* @file BackGround.h
* @brief 背景の処理
*/

#pragma once
#include "JsonLoader.h"

#include "FbxLoader.h"
#include "FbxObject3d.h"

#include <map>

class BackGround
{
public:
	/**
	* 初期化
	*
	* @param[in] adjustPos_ 発生させる位置
	*/
	void Initialize(float adjustPos_);
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
	* ファイルを読み込みオブジェクトの配置
	* 
	* @param[in] adjustPos_ 発生させる位置
	*/
	void SetObject(float adjustPos_);
	/**
	* 配置データをロード
	*
	* @param[in] num 何番の配置データを読み込みこむか
	*/
	void LoadJson(int num);
	/**
	* オブジェクトの配置
	*
	* @param[in] adjustPos_ 発生させる位置
	*/
	void ArrangementObject(float adjustPos_);
	/**
	* オブジェクトの削除
	*/
	void DeleteObject();

	//ゲッター
	/**
	* @return float size取得
	*/
	float GetSize() { return size; }
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition() { return position_; }

private:

	//fbxモデル
	FbxModel* groundModel = nullptr;
	FbxModel* boxModel = nullptr;
	FbxModel* treeModel = nullptr;
	FbxModel* wallModel = nullptr;
	FbxModel* pillarModel = nullptr;

	//モデル配列
	std::map<std::string, FbxModel*> backGroundModels;
	std::vector<FbxObject3D*> backGroundObjects;

	//配置データ
	LevelData* levelData = nullptr;

	//全体のサイズ
	float size = 520.0f;
	//背景の中心の位置(groundの位置)
	XMFLOAT3 position_ = {};
};

