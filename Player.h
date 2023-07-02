#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#pragma once
class Player
{
public:
	//�V���O���g���C���X�^���X
	Player* GetInstance();
	Player();
	~Player();
	void Initialize(FbxModel* model);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void setPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void setRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void setScale(XMFLOAT3 sca) { this->scale_ = sca; }
private:
	//�A�t�B���ϊ����
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;

private:
	FbxObject3D* playerObject = nullptr;
};

