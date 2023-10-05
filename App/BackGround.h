#pragma once
#include "JsonLoader.h"

#include "FbxLoader.h"
#include "FbxObject3d.h"

#include <map>

class BackGround
{
public:
	//num = 何番の配置データを読み込みこむか,adjustPos = どのくらいずらすか
	void Initialize(float adjustPos);

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//オブジェクトの配置
	void SetObject(float adjustPos);
	//配置データをロード num = 何番の配置データを読み込みこむか
	void LoadJson(int num);
	//オブジェクトの配置 adjustPos = どのくらいずらすか
	void ArrangementObject(float adjustPos);

	//オブジェクトの削除
	void DeleteObject();

	//ゲッター
	float GetSize() { return size; }
	XMFLOAT3 GetPosition() { return position; }

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
	XMFLOAT3 position = {};
};

