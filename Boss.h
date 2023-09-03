#pragma once

#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"

#include "ParticleManager.h"
#include "BossBullet.h"

#include "Collision.h"

enum BossAction {
	BOSSNOTHING,
	BOSSNORMAL,
	BOSSHOMING,
	BOSSMOVINGX,
	BOSSMOVINGY,
	BOSSMOVINGDIA,
};

enum BossShotType {
	BOSSNOTSHOT,
	BOSSSTRAIGHTSHOT,
	BOSSHOMINGSHOT
};

enum BossMoveType {
	BOSSNOTMOVE,
	BOSSMOVEX,
	BOSSMOVEY,
	BOSSMOVEDIA
};

class Boss
{
public:
	//�V���O���g���C���X�^���X
	Boss* GetInstance();
	Boss();
	~Boss();
	void Initialize();
	void Update(XMFLOAT3 pPos, float pSpeed);
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�ړ�
	void Move();
	void MoveX();
	void MoveY();

	//�p�[�e�B�N��
	void InitializeParticle();
	void UpdateParticle();

	//�e
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//���Z�b�g
	void Reset();

	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	void SetISDesd(bool isDead) { this->isDead = isDead; }
	bool GetIsDead() { return isDead; }
	bool GetIsParticle() { return isParticle; }

	CollisionData GetColData();

	void SetAction(int action);
	void ChangeAction();

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
	XMMATRIX matWorld = {};

	//�����蔻��p
	XMFLOAT3 colSize_ = { 2.0f,2.0f,1.0f };
private:
	//FBX
	FbxObject3D* BossObject = nullptr;
	//���f��
	FbxModel* bossModel = nullptr;
	FbxModel* bossBulletModel = nullptr;

	//���S�t���O
	bool isDead = false;

	//�s��	 0 Nothing�@�������Ȃ�,1 normal ���ʂɂ��܂�ł��� 2 homing�@�z�[�~���O�e��ł�, 3�`5 �����Ȃ��琳�ʂɒe��ł�
	int action = 0;
	//�ˌ��^�C�v 0 �������Ȃ� 1 ���� 2 �v���C���[�Ɍ�������
	int shotType = 0;
	//�ړ��^�C�v 0 �ړ����Ȃ� 1 X�� 2 Y�� 3�΂�
	int moveType = 0;

	//�A�N�V�����ω�
	const int ActionCoolTime = 100;
	int actionCollTimer = 0;
	//�A�N�V������
	int actionSize = 6;

	//-------�ړ�-------
	//�ړ� true��+�Ɉړ�����false��-�Ɉړ���
	bool moveX = true;
	bool moveY = true;
	//�ړ�����
	float moveMaxX = 10;
	float moveMaxY = 10;
	//�ړ��X�s�[�h
	float moveSpeed = 0.1f;

	//----------�e----------
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<BossBullet>> bullets_;

	//�e�̔��˃N�[���^�C��
	const int BulletCoolTime = 50;
	int bulletCoolTimer = 0;

	//�e�̑��x
	float bulletSpeed = 1.0f;

	//�e��ł��n�߂�͈�,�v���C���[���牽�i�񂾋������炩
	const float ShotStart = 100.0f;
	float shotStartPos = 100.0f;


	//-----�p�[�e�B�N��-----
	ParticleManager* particle = nullptr;
	bool isParticle = false;

	//�p�[�e�B�N������
	const int ParticleTime = 30;
	int particleTimer = 0;

	//------�v���C���[------
	XMFLOAT3 playerPosition_ = {};
	float playerSpeed_ = 0.0f;
};
