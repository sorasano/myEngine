#include "Model.h"
#include "Object3D.h"
#include "Base/DirectXCommon.h"

#pragma once
class Player
{
public:
	//�V���O���g���C���X�^���X
	Player* GetInstance();
	Player();
	~Player();
	void Initialize(DirectXCommon* dx,Model* model);
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	void Draw();
	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void setPosition(XMFLOAT3 pos);
	void setRotation(XMFLOAT3 rot);
	void setScale(XMFLOAT3 sca);
private:
	DirectXCommon* dx_ = nullptr;
	Model* model_ = nullptr;
	std::unique_ptr<Object3D> object3d_;
	//�A�t�B���ϊ����
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
};

