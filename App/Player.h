#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"
#include "PlayerBullet.h"
#include "Sprite.h"

#pragma once
class Player
{
public:
	//�V���O���g���C���X�^���X
	Player* GetInstance();
	Player();
	~Player();
	void Initialize(Input* input);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void DrawSprite(ID3D12GraphicsCommandList* cmdList);

	//�X�s�[�h�A�b�v
	void SpeedUpByEnemy();
	//�X�s�[�h�_�E��
	void SpeedDownByEnemy();

	//�ړ�����
	void Move();

	//�e
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//���e�B�N��
	void UpdateRaticle();
	void MoveRaticle();

	//�X�s�[�hUI
	void UpdateSprite();

	//���Z�b�g
	void Reset();

	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; }
	XMFLOAT3 GetRotation() { return rotation_; }
	XMFLOAT3 GetScale() { return scale_; }
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	float GetSpeed() { return speedZ + addSpeed; }
	CollisionData GetColData();

	void SetIsInvincible(bool isInvincible) { this->isInvincible = isInvincible; }

	//�e
	int GetBulletSize() { return static_cast<int>(bullets_.size()); }
	CollisionData GetBulletColData(int i);
	void SetBulletIsDead(bool isDead,int i);

private:
	//�A�t�B���ϊ����
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld = {};

	//�����蔻��p
	XMFLOAT3 colSize_ = {1.0f,1.0f,1.0f};

private:
	//FBX
	FbxObject3D* playerObject = nullptr;
	//���f��
	FbxModel* playerModel = nullptr;
	FbxModel* playerBulletModel = nullptr;
	FbxModel* fReticleModel = nullptr;
	FbxModel* bReticleModel = nullptr;

	//����
	Input* input_ = nullptr;

	//�ړ��͈�
	const XMFLOAT2 MoveMax = { 20.0f,10.0f };
	//�ړ��X�s�[�h(xy)
	float speedXY = 0.5f;
	//�ړ��X�s�[�h(z)
	float speedZ = 0.5f;

	//��b�X�s�[�h�ȊO�̉����X�s�[�h
	float addSpeed = 2.0f;
	//�G��|�������̉�����
	float addSpeedByEnemy = 0.5f;
	//�G�ɔ�e�������̌���
	float subSpeedByEnemy = 0.5f;

	//�ő������
	const float MaxSpeed = 5.0f;

	//���G����
	bool isInvincible = false;
	const int InvincibleTime = 10;
	int invincibleTimer = 0;

	//--------���e�B�N��--------

	//���e�B�N�����W
	XMFLOAT3 reticlePosition_ = { 0,0,0 };
	//�ړ��͈�
	const XMFLOAT2 ReticleMoveMax = { 40.0f,15.0f };
	//�ړ��X�s�[�h(xy)
	float reticleSpeedXY = 1.0f;

	//�O(��)�̃��e�B�N��
	FbxObject3D* frontReticleObject = nullptr;

	//���(��O)�̃��e�B�N��
	FbxObject3D* backReticleObject = nullptr;

	//���@����̋���
	const float kDistancePlayerTo3DFrontReticle = 25.0f;
	const float kDistancePlayerTo3DBackReticle = kDistancePlayerTo3DFrontReticle / 2;

	XMFLOAT3 fRPosition_ = { 0,0,0 };
	XMFLOAT3 fRRotation_ = { 0,0,0 };
	XMFLOAT3 fRScale_ = { 1,1,1 };


	XMFLOAT3 bRPosition_ = { 0,0,0 };
	XMFLOAT3 bRRotation_ = { 0,0,0 };
	XMFLOAT3 bRScale_ = { 1,1,1 };

	//�x�N�g��
	Vector3 playerVec = {};
	Vector3 reticleVec = {};
	Vector3 playerToReticleVec = {};

	//-----�X�v���C�g------
	Sprite* speedSprite = nullptr;

	//�X�P�[����x���ϓ��Ay���Œ�
	XMFLOAT2 speedSpriteScale = {0,32};
	XMFLOAT2 speedSpritePosition = { window_width / 2,window_height - 64};

	//�X�v���C�g�̍��E���[���ǂ̂��炢�J���邩
	float speedSpriteXSpace = 64;

public:

	//�e
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	//�e�̔��˃N�[���^�C��
	const int BulletCoolTime = 5;
	int bulletCoolTimer = 0;

	//�e�̑��x
	float bulletSpeed = 0;
};

