#pragma once
#include "JsonLoader.h"

#include "FbxLoader.h"
#include "FbxObject3d.h"

#include <map>

class BackGround
{
public:
	//num = 何番の配置データを読み込みこむか,adjustPos = どのくらいずらすか
	void Initialize(int num,float adjustPos);

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//配置データをロード num = 何番の配置データを読み込みこむか
	void LoadJson(int num);
	//オブジェクトの配置 adjustPos = どのくらいずらすか
	void ArrangementObject(float adjustPos);

	//ゲッター
	float GetSize() { return size; }

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
	float size = 180.0f;
};

