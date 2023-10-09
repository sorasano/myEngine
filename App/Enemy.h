#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "EnemyBullet.h"

#include "Collision.h"

enum EnemyType {
	NOTHING,
	NORMAL,
	HOMING,
	MOVINGX,
	MOVINGY,
	MOVINGDIA,
};

enum ShotType {
	NOTSHOT,
	STRAIGHTSHOT,
	HOMINGSHOT
};

enum MoveType {
	NOTMOVE,
	MOVEX,
	MOVEY,
	MOVEDIA
};

class Enemy
{
public:
	//�V���O���g���C���X�^���X
	Enemy* GetInstance();
	Enemy();
	~Enemy();
	void Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel);
	void Update(XMFLOAT3 pPos,float pSpeed);
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�ړ�
	void Move();
	void MoveX();
	void MoveY();

	void Reflection();

	//��ʓ��ɒ�؂�����
	void StopInScreen();

	//�p�[�e�B�N��
	void InitializeParticle();
	void UpdateParticle();

	//�e
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	void SetISDesd(bool isDead) { this->isDead_ = isDead; }
	bool GetIsDead() { return isDead_; }
	bool GetIsParticle() { return isParticle_; }

	CollisionData GetColData();

	void SetType(int type);
	void SetStopInScreen(bool stopInScreen) { this->stopInScreen_ = stopInScreen; }

	//�e
	int GetBulletSize() { return static_cast<int>(bullets_.size()); }
	CollisionData GetBulletColData(int i);
	void SetBulletIsDead(bool isDead, int i);

private:
	//�A�t�B���ϊ����;
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld_ = {};

	//�����蔻��p
	XMFLOAT3 colSize_ = { 2.0f,2.0f,1.0f };
private:
	//FBX
	FbxObject3D* enemyObject_ = nullptr;

	//�G���	 0 Nothing�@�������Ȃ�,1 normal ���ʂɂ��܂�ł��� 2 homing�@�z�[�~���O�e��ł�, 3�`5 �����Ȃ��琳�ʂɒe��ł�
	int type_ = 0;
	//�ˌ��^�C�v 0 �������Ȃ� 1 ���� 2 �v���C���[�Ɍ�������
	int shotType_ = 0;
	//�ړ��^�C�v 0 �ړ����Ȃ� 1 X�� 2 Y�� 3�΂�
	int moveType_ = 0;

	//���S�t���O
	bool isDead_ = false;

	//��ʓ��Œ�؂��邩
	bool stopInScreen_ = false;
	//��ʓ��Œ�؂��Ă��邩
	bool isStopInScreen_ = false;
	//���b��؂��邩
	const int StopInScreenTime_ = 600;
	int stopInScreenTimer_ = 0;
	//�v���C���[����ǂ̈ʒu�ɒ�~���邩
	float stopInScreenPosition_ = 26.0f;

	//-------�ړ�-------
	//�ړ� true��+�Ɉړ�����false��-�Ɉړ���
	bool moveX_ = true;
	bool moveY_ = true;
	//�ړ�����
	float moveMaxX_ = 10;
	float moveMaxY_ = 10;
	//�ړ��X�s�[�h
	float moveSpeed_ = 0.1f;

	//----------�e----------
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	//�e�̔��˃N�[���^�C��
	const int BulletCoolTime_ = 50;
	int bulletCoolTimer_ = 0;

	//�e�̑��x
	float bulletSpeed_ = 1.0f;

	//�e��ł��n�߂�͈�,�v���C���[���牽�i�񂾋������炩
	const float ShotStart_ = 100.0f;
	float shotStartPos_ = 100.0f;


	//-----�p�[�e�B�N��-----
	ParticleManager* particle_ = nullptr;
	bool isParticle_ = false;

	//�p�[�e�B�N������
	const int ParticleTime_ = 30;
	int particleTimer_ = 0;
	
	//------�v���C���[------
	XMFLOAT3 playerPosition_ = {};
	float playerSpeed_ = 0.0f;
};

