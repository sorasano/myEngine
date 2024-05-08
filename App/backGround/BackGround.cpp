/**
* @file BackGround.cpp
* @brief 背景の処理
*/

#include "BackGround.h"

#include "Random.h"
#define PI 3.1415

BackGround::BackGround()
{
}

BackGround::~BackGround()
{
	DeleteObject();
}

void BackGround::Initialize(float adjustPos)
{
	//モデル読み込み
	groundModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("ground"));
	boxModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("box"));
	treeModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("tree"));
	wallModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("wall"));
	pillarModel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("pillar"));

	backGroundModels_.emplace(std::make_pair("ground",std::move(groundModel_)));
	backGroundModels_.emplace(std::make_pair("box", std::move(boxModel_)));
	backGroundModels_.emplace(std::make_pair("tree", std::move(treeModel_)));
	backGroundModels_.emplace(std::make_pair("wall", std::move(wallModel_)));
	backGroundModels_.emplace(std::make_pair("pillar", std::move(pillarModel_)));

	//-------背景配置------
	SetObject(adjustPos);
}

void BackGround::Update()
{
	for (auto& object : backGroundObjects_) {
		object->Update();
	}
}

void BackGround::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& object : backGroundObjects_) {
		object->Draw(cmdList);
	}
}

void BackGround::SetObject(float adjustPos)
{
	//ランダムで配置するマップを選択
	int num = static_cast<int>(Random(1.0f, 4.99f));

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
		levelData_ = JsonLoader::LoadFile("backGround1");
		break;
	case 2:
		levelData_ = JsonLoader::LoadFile("backGround2");
		break;
	case 3:
		levelData_ = JsonLoader::LoadFile("backGround3");
		break;
	case 4:
		levelData_ = JsonLoader::LoadFile("backGround4");
		break;
	}
}

void BackGround::ArrangementObject(float adjustPos)
{

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		FbxModel* model_ = nullptr;
		decltype(backGroundModels_)::iterator it = backGroundModels_.find(objectData.fileName);
		if (it != backGroundModels_.end()) {
			model_ = it->second.get();
		}

		// モデルを指定して3Dオブジェクトを生成
		FbxObject3D* newObject = new FbxObject3D;
		newObject->Initialize();
		newObject->SetModel(model_);

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
		DirectX::XMFLOAT3 scale_;
		DirectX::XMStoreFloat3(&scale_, objectData.scaling);
		newObject->SetScale(scale_);

		//更新
		newObject->Update();

		// 配列に登録
		backGroundObjects_.push_back(newObject);

		//オブジェクトがgroundの時そのポジションを中心とする
		if (it->first == "ground") {
			this->position_ = pos;
		}
	}
}

void BackGround::DeleteObject()
{
	for (auto& object : backGroundObjects_) {
		//要素の解放
		delete object;
	}
	//すべての要素をコンテナから取り除く
	backGroundObjects_.clear();
}
