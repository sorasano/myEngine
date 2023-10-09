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
	FBX_SAFE_DELETE(enemyObject_);
}

void Enemy::Initialize(FbxModel* EnemyModel, FbxModel* enemyBulletModel_)
{

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	enemyObject_ = new FbxObject3D;
	enemyObject_->Initialize();
	enemyObject_->SetModel(EnemyModel);

	this->bulletModel_ = enemyBulletModel_;

}

void Enemy::Update(XMFLOAT3 pPos, float pSpeed)
{
	//�v���C���[���X�V
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead_) {

		//��ʓ��ɒ�؂�����
		StopInScreen();

		//�ړ�
		if (moveType_ != NOTMOVE) {
			Move();
		}

		//�ˌ�
		if (shotType_ != NOTSHOT) {
			//�v���C���[�̃X�s�[�h�Ŕ��˂��n�߂���W��ύX
			shotStartPos_ = ShotStart_ * playerSpeed_;

			if (position_.z < playerPosition_.z + shotStartPos_) {
				BulletUpdate();
			}
		}
	}
	//�p�[�e�B�N��
	else if (isParticle_) {
		UpdateParticle();
	}

	enemyObject_->SetPosition(position_);
	enemyObject_->SetScale(scale_);
	enemyObject_->SetRotate(rotation_);
	enemyObject_->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList_)
{
	if (!isDead_) {
		enemyObject_->Draw(cmdList_);

		//�e
		for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList_);
		}
	}
	//----�p�[�e�B�N��----
	else if (isParticle_) {
		particle_->Draw();
	}

}

void Enemy::Move()
{

	switch (moveType_)
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
	if (moveX_) {

		if (position_.x < moveMaxX_) {
			position_.x += moveSpeed_;
		}
		else {
			moveX_ = false;
		}

	}
	else {

		if (position_.x > -moveMaxX_) {
			position_.x -= moveSpeed_;
		}
		else {
			moveX_= true;
		}
	}
}

void Enemy::MoveY()
{
	if (moveY_) {

		if (position_.y < moveMaxY_) {
			position_.y += moveSpeed_;
		}
		else {
			moveY_ = false;
		}

	}
	else {

		if (position_.y > -moveMaxY_) {
			position_.y -= moveSpeed_;
		}
		else {
			moveY_ = true;
		}
	}
}

void Enemy::Reflection()
{
	//�ړ��𔽑Ό����ɂ�����
	if (moveType_ != NOTMOVE) {
		if (moveX_) {
			moveX_ = false;
		}
		else {
			moveX_= true;
		}

		if (moveY_) {
			moveY_ = false;
		}
		else {
			moveY_ = true;
		}
	}
}

void Enemy::StopInScreen()
{

	//stopInScreen��true�̏ꍇ�A�v���C���[����stopInScreenPosition�i�񂾋����ɓ��B�������؃t���O���I����
	if (stopInScreen_ && position_.z < playerPosition_.z + stopInScreenPosition_) {
		isStopInScreen_ = true;
		stopInScreen_ = false;
	}

	//��؃t���O���I���̏ꍇ
	if(isStopInScreen_){

		//���@�ɂ��Ă���
		position_.z += playerSpeed_;

		//�^�C�}�[��i�߂�
		stopInScreenTimer_++;
		if (stopInScreenTimer_ > StopInScreenTime_) {
			isStopInScreen_ = false;
		}
	}

}

void Enemy::InitializeParticle()
{
	//�t���O��true��
	isParticle_ = true;
	//�^�C�}�[�Z�b�g
	particleTimer_ = ParticleTime_;

	//�p�[�e�B�N������
	particle_ = new ParticleManager();
	particle_->Initialize("Resources/effect/effect1.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Z���ׂ�position����[+1.0f,-1.0f]�Ń����_���ɕ��z

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
		particle_->Add(ParticleTime_, pos, vel, acc);

	}

	particle_->Update();
}

void Enemy::UpdateParticle()
{

	//particle�L�����Ԃ��߂�����t���O��false��
	if (particleTimer_ > 0) {
		particleTimer_--;
	}
	else {
		isParticle_ = false;
	}

	if (isParticle_) {
		particle_->Update();
	}
}

void Enemy::Shot()
{
	bulletCoolTimer_++;

	if (BulletCoolTime_ < bulletCoolTimer_) {
		MakeBullet();

		bulletCoolTimer_ = 0;
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

	switch (shotType_)
	{
	case NOTSHOT:

		break;

	case STRAIGHTSHOT:
		//z����-�����̒P�ʃx�N�g���ɑ��x��������
		velocity = { 0.0f,0.0f,-1.0f };
		velocity *= bulletSpeed_;
		break;

	case HOMINGSHOT:

		//���@�ƓG�̃x�N�g�������
		Vector3 playerVec_ = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
		Vector3 enemyVec = { position_.x,position_.y,position_.z };

		velocity = playerVec_ - enemyVec;

		//���K�������đ��x��������
		velocity.normalize();
		velocity *= bulletSpeed_;

		velocity.z += playerSpeed_;

		break;
	}

	//�e�̐���
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity, playerSpeed_);
	bullets_.push_back(std::move(newBullet));
}

CollisionData Enemy::GetColData()
{

	CollisionData colData;

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Enemy::SetType(int type)
{

	this->type_ = type;

	switch (type)
	{
	case NOTHING:
		shotType_ = NOTSHOT;
		moveType_ = NOTMOVE;
		break;

	case NORMAL:
		shotType_ = STRAIGHTSHOT;
		moveType_ = NOTMOVE;
		break;

	case HOMING:
		shotType_ = HOMINGSHOT;
		moveType_ = NOTMOVE;
		break;

	case MOVINGX:
		shotType_= STRAIGHTSHOT;
		moveType_ = MOVEX;
		break;

	case MOVINGY:
		shotType_ = STRAIGHTSHOT;
		moveType_= MOVEY;
		break;

	case MOVINGDIA:
		shotType_ = STRAIGHTSHOT;
		moveType_ = MOVEDIA;
		break;
	}
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

