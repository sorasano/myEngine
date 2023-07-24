#include "BackGround.h"

#include "Random.h"
#define PI 3.1415

void BackGround::Initialize(float adjustPos)
{
	//モデル読み込み
	groundModel = FbxLoader::GetInstance()->LoadModelFromFile("ground");
	boxModel = FbxLoader::GetInstance()->LoadModelFromFile("box");
	treeModel = FbxLoader::GetInstance()->LoadModelFromFile("tree");
	wallModel = FbxLoader::GetInstance()->LoadModelFromFile("wall");
	pillarModel = FbxLoader::GetInstance()->LoadModelFromFile("pillar");

	backGroundModels.insert(std::make_pair("ground", groundModel));
	backGroundModels.insert(std::make_pair("box", boxModel));
	backGroundModels.insert(std::make_pair("tree", treeModel));
	backGroundModels.insert(std::make_pair("wall", wallModel));
	backGroundModels.insert(std::make_pair("pillar", pillarModel));

	//-------背景配置------
	SetObject(adjustPos);
}

void BackGround::Update()
{
	for (auto& object : backGroundObjects) {
		object->Update();
	}
}

void BackGround::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& object : backGroundObjects) {
		object->Draw(cmdList);
	}
}

void BackGround::SetObject(float adjustPos)
{
	//ランダムで配置するマップを選択
	int num = static_cast<int>(Random(1.0f,4.99f));

	//jsonファイル読み込み
	LoadJson(num);
	//オブジェクトの配置
	ArrangementObject(adjustPos);
}

void BackGround::LoadJson(int num)
{
	// レベルデータの読み込み
	switch (num)
	{
	case 1:
		levelData = JsonLoader::LoadFile("backGround1");
		break;
	case 2:
		levelData = JsonLoader::LoadFile("backGround2");
		break;
	case 3:
		levelData = JsonLoader::LoadFile("backGround3");
		break;
	case 4:
		levelData = JsonLoader::LoadFile("backGround4");
		break;
	}
}

void BackGround::ArrangementObject(float adjustPos)
{

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		FbxModel* model = nullptr;
		decltype(backGroundModels)::iterator it = backGroundModels.find(objectData.fileName);
		if (it != backGroundModels.end()) {
			model = it->second;
		}

		// モデルを指定して3Dオブジェクトを生成
		FbxObject3D* newObject = new FbxObject3D;
		newObject->Initialize();
		newObject->SetModel(model);

		// 座標
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, objectData.translation);
		//下に配置
		pos.y -= 12.0f;
		//調整
		pos.z += adjustPos;
		newObject->SetPosition(pos);

		// 回転角
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, objectData.rotation);
		rot.x = static_cast<float>(90 * (PI / 180));
		newObject->SetRotate(rot);

		// サイズ
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, objectData.scaling);
		newObject->SetScale(scale);

		//更新
		newObject->Update();

		// 配列に登録
		backGroundObjects.push_back(newObject);

		//オブジェクトがgroundの時そのポジションを中心とする
		if (it->first == "ground") {
			this->position = pos;
		}
	}
}

void BackGround::DeleteObject()
{
	for (auto& object : backGroundObjects) {
		//要素の解放
		delete object;
	}
	//すべての要素をコンテナから取り除く
	backGroundObjects.clear();
}
