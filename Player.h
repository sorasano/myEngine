#include "FbxLoader.h"
#include "FbxObject3D.h"
#include "DirectXCommon.h"
#include "PlayerBullet.h"

#pragma once
class Player
{
public:
	//�V���O���g���C���X�^���X
	Player* GetInstance();
	Player();
	~Player();
	void Initialize(Input* input, FbxModel* playerModel, FbxModel* playerBulletModel);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�X�s�[�h�A�b�v
	void SpeedUpByEnemy();
	//�X�s�[�h�_�E��
	void SpeedDownByEnemy();

	//�e
	void Move();
	void Shot();
	void BulletUpdate();
	void MakeBullet();

	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; }
	XMFLOAT3 GetRotation() { return rotation_; }
	XMFLOAT3 GetScale() { return scale_; }
	void SetPosition(XMFLOAT3 pos) { this->position_ = pos; }
	void SetRotation(XMFLOAT3 rot) { this->rotation_ = rot; }
	void SetScale(XMFLOAT3 sca) { this->scale_ = sca; }

	XMFLOAT3 GetColSize(){ return colSize; }
	//�e
	int GetBulletSize() { return static_cast<int>(playerBullet_.size()); }
	XMFLOAT3 GetBulletPosition(int i);
	XMFLOAT3 GetBulletRotation(int i);
	XMFLOAT3 GetBulletScale(int i);
	XMFLOAT3 GetBulletColSize(int i);

	void SetBulletIsDead(bool isDead,int i);
	void SetIsInvincible(bool isInvincible){ this->isInvincible = isInvincible; }

private:
	//�A�t�B���ϊ����
	XMFLOAT3 position_ = { 0,0,0 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 scale_ = { 1,1,1 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld = {};

	//�����蔻��p
	XMFLOAT3 colSize = {1.0f,1.0f,1.0f};

private:
	//FBX
	FbxObject3D* playerObject = nullptr;
	//����
	Input* input_ = nullptr;

	//�ړ��͈�
	const XMFLOAT2 MoveMax = { 20.0f,10.0f };
	//�ړ��X�s�[�h(xy)
	float speedXY = 0.1f;
	//�ړ��X�s�[�h(z)
	float speedZ = 0.5f;

	//��b�X�s�[�h�ȊO�̉����X�s�[�h
	float addSpeed = 0.0f;
	//�G��|�������̉�����
	float addSpeedByEnemy = 0.8f;
	//�G�ɔ�e�������̌���
	float subSpeedByEnemy = 0.8f;

	//���G����
	bool isInvincible = false;
	const int InvincibleTime = 30;
	int invincibleTimer = 0;

public:

	//�e
	FbxModel* playerBulletModel_ = nullptr;
	std::list<std::unique_ptr<PlayerBullet>> playerBullet_;

	//�e�̔��˃N�[���^�C��
	const int BulletCoolTime = 30;
	int bulletCoolTimer = 0;

};

