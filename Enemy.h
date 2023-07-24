#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "EnemyBullet.h"

enum EnemyType {
	NOTHING,
	NORMAL,
	HOMING,
	MOVING,
};

class Enemy
{
public:
	//�V���O���g���C���X�^���X
	Enemy* GetInstance();
	Enemy();
	~Enemy();
	void Initialize(FbxModel* EnemyModel);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void UpdateNothing();
	void UpdateNormal();
	void UpdateHoming();
	void UpdateMoving();

	bool Collition(XMFLOAT3 pos, XMFLOAT3 size);

	void InitializeParticle();
	void UpdateParticle();

	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	bool GetIsDead() { return isDead; }
private:
	//�A�t�B���ϊ����;
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld = {};

private:
	//FBX
	FbxObject3D* enemyObject = nullptr;

	//���S�t���O
	bool isDead = false;

	//0 = Nothing�@�������Ȃ�,1 = normal ���ʂɂ��܂�ł��� 2 = homing�@�z�[�~���O�e��ł�, 3 = �����Ȃ��琳�ʂɒe��ł�
	int type = 0;
	//��ʓ��Œ�؂��邩
	bool stopInScreen = false;

	//----------�e----------
	FbxModel* enemyBulletModel_ = nullptr;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullet_;

	//�e�̔��˃N�[���^�C��
	const int BulletCoolTime = 10;
	int bulletCoolTimer = 0;

	//-----�p�[�e�B�N��-----
	ParticleManager* particle = nullptr;
	bool isParticle = false;

	//�p�[�e�B�N������
	const int ParticleTime = 30;
	int particleTimer = 0;

};

