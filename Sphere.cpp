#include "Sphere.h"

Sphere* Sphere::GetInstance()
{
	static Sphere instance;
	return &instance;
}

Sphere::Sphere()
{
}

Sphere::~Sphere()
{
}

void Sphere::Initialize(DirectXCommon* dx, Model* model)
{
	//引数から受け取った値を代入
	this->dx_ = dx;
	this->model_ = model;

	//オブジェクト初期化
	Object3D* newObject = new Object3D();
	newObject->Initialize(dx_, model_);
	object3d_.reset(newObject);
}

void Sphere::Update(XMMATRIX& matView, XMMATRIX& matProjection)
{
	//position_.x += 0.02;

	object3d_->setPosition(position_);
	object3d_->setRotation(rotation_);
	object3d_->setScale(scale_);
	//オブジェクト更新
	object3d_->Update(matView, matProjection);
}

void Sphere::Draw()
{
	object3d_->Draw(model_->vbView, model_->ibView);
}

void Sphere::setPosition(XMFLOAT3 pos)
{
	position_ = pos;
}

void Sphere::setRotation(XMFLOAT3 rot)
{
	rotation_ = rot;
}

void Sphere::setScale(XMFLOAT3 sca)
{
	scale_ = sca;
}
