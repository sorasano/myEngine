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

void Enemy::Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	enemyObject = new FbxObject3D;
	enemyObject->Initialize();
	enemyObject->SetModel(EnemyModel);

	this->bulletModel_ = enemyBulletModel;

	type = MOVINGDIA;

	switch (type)
	{
	case NOTHING:
		shotType = NOTSHOT;
		moveType = NOTMOVE;
		break;

	case NORMAL:
		shotType = STRAIGHTSHOT;
		moveType = NOTMOVE;
		break;

	case HOMING:
		shotType = HOMINGSHOT;
		moveType = NOTMOVE;
		break;

	case MOVINGX:
		shotType = STRAIGHTSHOT;
		moveType = MOVEX;
		break;

	case MOVINGY:
		shotType = STRAIGHTSHOT;
		moveType = MOVEY;
		break;

	case MOVINGDIA:
		shotType = STRAIGHTSHOT;
		moveType = MOVEDIA;
		break;
	}
}

void Enemy::Update(XMFLOAT3 pPos, float pSpeed)
{
	//�v���C���[���X�V
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead) {

		//��ʓ��ɒ�؂�����
		StopInScreen();

		//�ړ�
		if (moveType != NOTMOVE) {
			Move();
		}

		//�ˌ�
		if (shotType != NOTSHOT) {
			//�v���C���[�̃X�s�[�h�Ŕ��˂��n�߂���W��ύX
			shotStartPos = ShotStart * playerSpeed_;

			if (position_.z < playerPosition_.z + shotStartPos) {
				BulletUpdate();
			}
		}
	}
	//�p�[�e�B�N��
	else if (isParticle) {
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

		//�e
		for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}
	//----�p�[�e�B�N��----
	else if (isParticle) {
		particle->Draw();
	}

}

void Enemy::Move()
{

	switch (moveType)
	{
	case NOTMOVE:
		break;

	case MOVEX:
		MoveX();
		break;

	case MOVEY:
		MoveY();
		break;

	case MOVEDIA:
		MoveX();
		MoveY();
		break;
	}

}

void Enemy::MoveX()
{
	if (moveX) {

		if (position_.x < moveMaxX) {
			position_.x += moveSpeed;
		}
		else {
			moveX = false;
		}

	}
	else {

		if (position_.x > -moveMaxX) {
			position_.x -= moveSpeed;
		}
		else {
			moveX = true;
		}
	}
}

void Enemy::MoveY()
{
	if (moveY) {

		if (position_.y < moveMaxY) {
			position_.y += moveSpeed;
		}
		else {
			moveY = false;
		}

	}
	else {

		if (position_.y > -moveMaxY) {
			position_.y -= moveSpeed;
		}
		else {
			moveY = true;
		}
	}
}

void Enemy::Reflection()
{
	//�ړ��𔽑Ό����ɂ�����
	if (moveType != NOTMOVE) {
		if (moveX) {
			moveX = false;
		}
		else {
			moveX = true;
		}

		if (moveY) {
			moveY = false;
		}
		else {
			moveY = true;
		}
	}
}

void Enemy::StopInScreen()
{

	if (stopInScreen) {

		//�v���C���[����stopInScreenPosition�i�񂾋����ɓ��B������
		if (position_.z < playerPosition_.z + stopInScreenPosition) {

			//���@�ɂ��Ă���
			position_.z += playerSpeed_;

			//�^�C�}�[��i�߂�
			stopInScreenTimer++;
			if (stopInScreenTimer > StopInScreenTime) {
				stopInScreen = false;
			}

		}

	}

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
		vel.x = Random(-md_vel, md_vel);
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

void Enemy::Shot()
{
	bulletCoolTimer++;

	if (BulletCoolTime < bulletCoolTimer) {
		MakeBullet();

		bulletCoolTimer = 0;
	}

}

void Enemy::BulletUpdate()
{
	//�v���C���[���O�ɂ���G�̒e�̂ݔ���
	if (position_.z > playerPosition_.z) {
		Shot();
	}

	//�G�̒e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {return bullet->GetIsDead(); });
}

void Enemy::MakeBullet()
{
	Vector3 velocity = {};

	switch (shotType)
	{
	case NOTSHOT:

		break;

	case STRAIGHTSHOT:
		//z����-�����̒P�ʃx�N�g���ɑ��x��������
		velocity = { 0.0f,0.0f,-1.0f };
		velocity *= bulletSpeed;
		break;

	case HOMINGSHOT:

		//���@�ƓG�̃x�N�g�������
		Vector3 playerVec = { playerPosition_.x ,playerPosition_.y,playerPosition_.z};
		Vector3 enemyVec = { position_.x,position_.y,position_.z };

		velocity = playerVec - enemyVec;

		//���K�������đ��x��������
		velocity.normalize();
		velocity *= bulletSpeed;

		velocity.z += playerSpeed_;

		break;
	}

	//�e�̐���
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity,playerSpeed_);
	bullets_.push_back(std::move(newBullet));
}


CollisionData Enemy::GetColData()
{

	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}

CollisionData Enemy::GetBulletColData(int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

void Enemy::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}

