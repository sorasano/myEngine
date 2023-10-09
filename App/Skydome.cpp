#include "Skydome.h"
#define PI 3.1415

void Skydome::Initialize()
{
	skydomeModel = FbxLoader::GetInstance()->LoadModelFromFile("skydome");
	//3dオブジェクト生成とモデルのセット
	skydomeObject = new FbxObject3D;
	skydomeObject->Initialize();
	skydomeObject->SetModel(skydomeModel);

	//ラジアン変換
	rotation_.x = static_cast<float>(90 * (PI / 180));
	rotation_.z = static_cast<float>(120 * (PI / 180));
}

void Skydome::Update(float eyeZ)
{
	position_.z = eyeZ;

	skydomeObject->SetPosition(position_);
	skydomeObject->SetScale(scale_);
	skydomeObject->SetRotate(rotation_);
	skydomeObject->Update();
}

void Skydome::Draw(ID3D12GraphicsCommandList* cmdList_)
{
	skydomeObject->Draw(cmdList_);
}
