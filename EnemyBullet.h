#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "Vector3.h"

#pragma once
class EnemyBullet
{
public:
	//�V���O���g���C���X�^���X
	EnemyBullet* GetInstance();
	EnemyBullet();
	~EnemyBullet();
	void Initialize(FbxModel* model, XMFLOAT3 position, Vector3 velocity);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void Move();
	void OnCollision();
	bool GetIsDead()const { return isDead_; }
	void SetIsDead(bool isDead) { this->isDead_ = isDead; };

	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	XMFLOAT3 GetColSize() { return colSize_; };

private:
	//�A�t�B���ϊ����
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld = {};

	//�����蔻��p
	XMFLOAT3 colSize_ = { 0.2f,0.2f,1.0f };

private:
	//FBX
	FbxObject3D* EnemyBulletObject = nullptr;

	//�ړ��x�N�g��
	Vector3 velocity_ = {};

	//����
	static const int32_t lifeTime = 60 * 5;
	//�f�X�^�C�}�[
	int32_t deathTimer_ = lifeTime;
	//�f�X�t���O
	bool isDead_ = false;

};

