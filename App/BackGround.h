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
	* コンストラクタ
	*/
	BackGround();
	/**
	* デストラクタ
	*/
	~BackGround();
	/**
	* 初期化
	*
	* @param[in] adjustPos_ 発生させる位置
	*/
	void Initialize(float adjustPos);
	/**
	* 更新
	*/
	void Update();
	/**
	* 描画
	*
	* @param[out] cmdList_ コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/**
	* ファイルを読み込みオブジェクトの配置
	* 
	* @param[in] adjustPos_ 発生させる位置
	*/
	void SetObject(float adjustPos);
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
	void ArrangementObject(float adjustPos);
	/**
	* オブジェクトの削除
	*/
	void DeleteObject();

	//ゲッター
	/**
	* @return float size取得
	*/
	float GetSize() { return size_; }
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition() { return position_; }

private:

	//fbxモデル
	std::unique_ptr<FbxModel> groundModel_ = nullptr;
	std::unique_ptr<FbxModel>  boxModel_ = nullptr;
	std::unique_ptr<FbxModel>  treeModel_ = nullptr;
	std::unique_ptr<FbxModel>  wallModel_ = nullptr;
	std::unique_ptr<FbxModel>  pillarModel_ = nullptr;

	//モデル配列
	std::map<std::string, std::unique_ptr<FbxModel>> backGroundModels_;
	std::vector<FbxObject3D*> backGroundObjects_;

	//配置データ
	LevelData* levelData_ = nullptr;

	//全体のサイズ
	float size_ = 520.0f;
	//背景の中心の位置(groundの位置)
	XMFLOAT3 position_ = {};
};