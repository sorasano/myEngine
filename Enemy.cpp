#include "Enemy.h"
#include "Random.h"

Enemy* Enemy::GetInstance()
{
	static Enemy instance;
	return &instance;
}

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	FBX_SAFE_DELETE(enemyObject);
}

void Enemy::Initialize(FbxModel* EnemyModel)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	enemyObject = new FbxObject3D;
	enemyObject->Initialize();
	enemyObject->SetModel(EnemyModel);

}

void Enemy::Update()
{
	switch (type)
	{
	case NOTHING:
		UpdateNothing();
		break;

	case NORMAL:
		UpdateNormal();
		break;

	case HOMING:
		UpdateHoming();
		break;

	case MOVING:
		UpdateMoving();
		break;
	}

	//�p�[�e�B�N��
	if (isParticle) {
		UpdateParticle();
	}

	enemyObject->SetPosition(position_);
	enemyObject->SetScale(scale_);
	enemyObject->SetRotate(rotation_);
	enemyObject->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead) {
		enemyObject->Draw(cmdList);
	}

	//----�p�[�e�B�N��----
	if (isParticle) {
		particle->Draw();
	}

}

void Enemy::UpdateNothing()
{
}

void Enemy::UpdateNormal()
{
}

void Enemy::UpdateHoming()
{
}

void Enemy::UpdateMoving()
{
}

void Enemy::InitializeParticle()
{
	//�t���O��true��
	isParticle = true;
	//�^�C�}�[�Z�b�g
	particleTimer = ParticleTime;

	//�p�[�e�B�N������
	particle = new ParticleManager();
	particle->Initialize("Resources/effect1.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Z���ׂ�position����[+1.0f,-1.0f]�Ń����_���ɕ��z
		const float md_pos = 1.0f;

		XMFLOAT3 pos{};
		pos.x = Random(position_.x - 1.0f, position_.x + 1.0f);
		pos.y = Random(position_.y - 1.0f, position_.y + 1.0f);
		pos.z = Random(position_.z - 1.0f, position_.z + 1.0f);

		//X,Y,Z���ׂ�[-0.05f,+0.05f]�Ń����_���ɕ��z
		const float md_vel = 0.05f;
		XMFLOAT3 vel{};
		vel.x = Random(-md_vel,md_vel);
		vel.y = Random(-md_vel, md_vel);
		vel.z = Random(-md_vel, md_vel);

		//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
		XMFLOAT3 acc{};
		const float md_acc = -0.001f;
		acc.y = Random(md_acc, 0);

		//�ǉ�
		particle->Add(ParticleTime, pos, vel, acc);

	}

	particle->Update();
}

void Enemy::UpdateParticle()
{

	//particle�L�����Ԃ��߂�����t���O��false��
	if (particleTimer > 0) {
		particleTimer--;
	}
	else {
		isParticle = false;
	}

	if (isParticle) {
		particle->Update();
	}
}

bool Enemy::Collition(XMFLOAT3 pos, XMFLOAT3 size)
{
	XMFLOAT3 ePos = position_;
	XMFLOAT3 eSize = scale_;

	float ePosX1 = ePos.x - eSize.x;
	float ePosX2 = ePos.x + eSize.x;

	float ePosY1 = ePos.y - eSize.y;
	float ePosY2 = ePos.y + eSize.y;

	float ePosZ1 = ePos.z - eSize.z;
	float ePosZ2 = ePos.z + eSize.z;

	XMFLOAT3 bPos = pos;
	XMFLOAT3 bSize = size;

	float bPosX1 = bPos.x - bSize.x;
	float bPosX2 = bPos.x + bSize.x;

	float bPosY1 = bPos.y - bSize.y;
	float bPosY2 = bPos.y + bSize.y;

	float bPosZ1 = bPos.z - bSize.z;
	float bPosZ2 = bPos.z + bSize.z;


	if (ePosX1 < bPosX2 && bPosX1 < ePosX2) {
		if (ePosY1 < bPosY2 && bPosY1 < ePosY2) {
			if (ePosZ1 < bPosZ2 && bPosZ1 < ePosZ2) {

				//���@�̒e�ɓ��������玀�S
				isDead = true;

				//�p�[�e�B�N������
				InitializeParticle();
			}
		}
	}

	return isDead;
}
