#include "Shadow.h"

Shadow::Shadow() {

}

void Shadow::Initialize(FbxModel* model)
{
	//モデルのセット
	shadowModel_.reset(model);

	//3dオブジェクト生成とモデルのセット
	std::unique_ptr<FbxObject3D> newShadowObject = std::make_unique<FbxObject3D>();
	newShadowObject->Initialize();
	newShadowObject->SetModel(shadowModel_.get());
	shadowObject_.swap(newShadowObject);

}

void Shadow::Update(const XMFLOAT3 pos, const XMFLOAT3 rot)
{
	//座標更新
	position_.x = pos.x;
	position_.y = groundPos_;
	position_.z = pos.z;

	//回転更新
	this->rotation_ = rot;

	//行列更新
	UpdateMatrix();
}

void Shadow::UpdateMatrix()
{
	shadowObject_->SetPosition(position_);
	shadowObject_->SetScale(scale_);
	shadowObject_->SetRotate(rotation_);
	shadowObject_->Update();
}

void Shadow::Draw(ID3D12GraphicsCommandList* cmdList)
{
	shadowObject_->Draw(cmdList);
}

void Shadow::SetModel(FbxModel* EnemyModel)
{
	shadowObject_->SetModel(EnemyModel);
}
