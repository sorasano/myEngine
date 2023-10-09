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

	float GetSpeed() { return speedZ_ + addSpeed_; }
	CollisionData GetColData();

	void SetIsInvincible(bool isInvincible) { this->isInvincible_ = isInvincible; }

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
	XMMATRIX matWorld_ = {};

	//�����蔻��p
	XMFLOAT3 colSize_ = {1.0f,1.0f,1.0f};

private:
	//FBX
	FbxObject3D* playerObject_ = nullptr;
	//���f��
	FbxModel* playerModel_ = nullptr;
	FbxModel* playerBulletModel_ = nullptr;
	FbxModel* fReticleModel_ = nullptr;
	FbxModel* bReticleModel_ = nullptr;

	//����
	Input* input_ = nullptr;

	//�ړ��͈�
	const XMFLOAT2 MoveMax_ = { 20.0f,10.0f };
	//�ړ��X�s�[�h(xy)
	float speedXY_ = 0.5f;
	//�ړ��X�s�[�h(z)
	float speedZ_ = 0.5f;

	//��b�X�s�[�h�ȊO�̉����X�s�[�h
	float addSpeed_ = 0.0f;
	//�G��|�������̉�����
	float addSpeedByEnemy_ = 0.5f;
	//�G�ɔ�e�������̌���
	float subSpeedByEnemy_ = 0.5f;

	//�ő������
	const float MaxSpeed_ = 5.0f;

	//���G����
	bool isInvincible_ = false;
	const int InvincibleTime_ = 10;
	int invincibleTimer_ = 0;

	//--------���e�B�N��--------

	//���e�B�N�����W
	XMFLOAT3 reticlePosition_ = { 0,0,0 };
	//�ړ��͈�
	const XMFLOAT2 ReticleMoveMax_ = { 40.0f,15.0f };
	//�ړ��X�s�[�h(xy)
	float reticleSpeedXY_ = 1.0f;

	//�O(��)�̃��e�B�N��
	FbxObject3D* frontReticleObject_ = nullptr;

	//���(��O)�̃��e�B�N��
	FbxObject3D* backReticleObject_ = nullptr;

	//���@����̋���
	const float kDistancePlayerTo3DFrontReticle_ = 25.0f;
	const float kDistancePlayerTo3DBackReticle_ = kDistancePlayerTo3DFrontReticle_ / 2;

	XMFLOAT3 fRPosition_ = { 0,0,0 };
	XMFLOAT3 fRRotation_ = { 0,0,0 };
	XMFLOAT3 fRScale_ = { 1,1,1 };


	XMFLOAT3 bRPosition_ = { 0,0,0 };
	XMFLOAT3 bRRotation_ = { 0,0,0 };
	XMFLOAT3 bRScale_ = { 1,1,1 };

	//�x�N�g��
	Vector3 playerVec_ = {};
	Vector3 reticleVec_ = {};
	Vector3 playerToReticleVec_ = {};

	//-----�X�v���C�g------
	Sprite* speedSprite_ = nullptr;

	//�X�P�[����x���ϓ��Ay���Œ�
	XMFLOAT2 speedSpriteScale_ = {0,32};
	XMFLOAT2 speedSpritePosition_ = { window_width / 2,window_height - 64};

	//�X�v���C�g�̍��E���[���ǂ̂��炢�J���邩
	float speedSpriteXSpace_ = 64;

private:

	//�e
	FbxModel* bulletModel_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	//�e�̔��˃N�[���^�C��
	const int BulletCoolTime_ = 5;
	int bulletCoolTimer_ = 0;

	//�e�̑��x
	float bulletSpeed_ = 0;
};

