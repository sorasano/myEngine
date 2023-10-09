#include "Boss.h"
#include "Random.h"

Boss* Boss::GetInstance()
{
	static Boss instance;
	return &instance;
}

Boss::Boss()
{
}

Boss::~Boss()
{
	FBX_SAFE_DELETE(BossObject_);
}

void Boss::Initialize()
{
	//���f�������w�肵�ăt�@�C���ǂݍ���
	normalBossModel_ = FbxLoader::GetInstance()->LoadModelFromFile("normalBoss");
	hardBossModel_ = FbxLoader::GetInstance()->LoadModelFromFile("hardBoss");

	bossBulletModel_ = FbxLoader::GetInstance()->LoadModelFromFile("bossBullet");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	BossObject_ = new FbxObject3D;
	BossObject_->Initialize();
	BossObject_->SetModel(normalBossModel_);

	this->bulletModel_ = bossBulletModel_;

}

void Boss::Update(XMFLOAT3 pPos, float pSpeed)
{
	//�v���C���[���X�V
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead_) {

		//�s���ω�
		actionCollTimer_++;
		if (actionCollTimer_ > ActionCoolTime_) {
			actionCollTimer_ = 0;
			ChangeAction();
		}

		//�ړ�
		Move();

		//�ˌ�
		if (shotType_ != BOSSNOTSHOT) {
			//�v���C���[�̃X�s�[�h�Ŕ��˂��n�߂���W��ύX
			shotStartPos_ = ShotStart_ * playerSpeed_;
			if (position_.z < playerPosition_.z + shotStartPos_) {
				BulletUpdate();
			}
		}

	}

	//�p�[�e�B�N��
	if (isParticle_) {
		UpdateParticle();
	}

	BossObject_->SetPosition(position_);
	BossObject_->SetScale(scale_);
	BossObject_->SetRotate(rotation_);
	BossObject_->Update();
}

void Boss::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead_) {
		BossObject_->Draw(cmdList);

		//�e
		for (std::unique_ptr<BossBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}
	//----�p�[�e�B�N��----
	if (isParticle_) {
		particle_->Draw();
	}

}

void Boss::Move()
{
	//���@�ɒǏ]
	position_.z += playerSpeed_;

	switch (moveType_)
	{
	case BOSSNOTMOVE:
		break;

	case BOSSMOVEX:
		MoveX();
		break;

	case BOSSMOVEY:
		MoveY();
		break;

	case BOSSMOVEDIA:
		MoveX();
		MoveY();
		break;
	}

}

void Boss::MoveX()
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
			moveX_ = true;
		}
	}
}

void Boss::MoveY()
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


void Boss::InitializeParticle()
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

void Boss::UpdateParticle()
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

void Boss::Shot()
{

	//�N�[���^�C���Œe�̔��ˊԊu�𒲐�
	bulletCoolTimer_++;

	if (BulletCoolTime_ < bulletCoolTimer_) {

		//�ˏo�p�x���v�Z���e�̔���
		Vector3 velocity = {};
		Vector3 playerVec_ = {};
		Vector3 BossVec = {};

		switch (shotType_)
		{
		case BOSSNOTSHOT:

			break;

		case BOSSSTRAIGHTSHOT:
			//z����-�����̒P�ʃx�N�g���ɑ��x��������
			velocity = { 0.0f,0.0f,-1.0f };
			velocity *= bulletSpeed_;

			//�e�̐���
			MakeBullet(velocity);
			break;

		case BOSSHOMINGSHOT:

			//���@�ƓG�̃x�N�g�������
			playerVec_ = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
			BossVec = { position_.x,position_.y,position_.z };

			velocity = playerVec_ - BossVec;

			//���K�������đ��x��������
			velocity.normalize();
			velocity *= bulletSpeed_;

			velocity.z += playerSpeed_;

			//�e�̐���
			MakeBullet(velocity);
			break;

		case BOSSMULTISHOT:

			//����
			velocity = { 0.0f,0.1f,-1.0f };
			velocity *= bulletSpeed_;
			//�e�̐���
			MakeBullet(velocity);

			//��
			velocity = { 0.0f,0.1f,-1.0f };
			velocity *= bulletSpeed_;
			//�e�̐���
			MakeBullet(velocity);

			//��
			velocity = { 0.0f,-0.1f,-1.0f };
			velocity *= bulletSpeed_;
			//�e�̐���
			MakeBullet(velocity);

			//�E
			velocity = { 0.1f,0.0f,-1.0f };
			velocity *= bulletSpeed_;
			//�e�̐���
			MakeBullet(velocity);

			//��
			velocity = { -0.1f,0.0f,-1.0f };
			velocity *= bulletSpeed_;
			//�e�̐���
			MakeBullet(velocity);

			break;
		}

		bulletCoolTimer_ = 0;
	}

}

void Boss::BulletUpdate()
{
	//�v���C���[���O�ɂ���G�̒e�̂ݔ���
	if (position_.z > playerPosition_.z) {
		Shot();
	}

	//�G�̒e�X�V
	for (std::unique_ptr<BossBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<BossBullet>& bullet) {return bullet->GetIsDead(); });
}

void Boss::MakeBullet(Vector3 velocity)
{
	//�e�̐���
	std::unique_ptr<BossBullet> newBullet = std::make_unique<BossBullet>();
	newBullet->Initialize(bulletModel_, position_, velocity, playerSpeed_);
	bullets_.push_back(std::move(newBullet));

}

void Boss::Reset()
{
	position_ = { 0,0,0 };

	//�e
	bullets_.clear();

	//���S�t���O
	isDead_ = false;

	//2�i�K�ڈڍs�t���O
	isBossHardMode_ = false;

	//hp
	hp_ = 10;

	particleTimer_ = 0;
}

CollisionData Boss::GetColData()
{

	CollisionData colData;

	colData.position_ = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Boss::HitBullet()
{
	//hp������Ȃ猸�炷�A�Ȃ��Ȃ玀�S
	if (hp_) {
		hp_--;
	}
	else {
		isDead_ = true;
	}

	//hp��2�i�K�ڈڍs�^�C�~���O�ɂȂ�����2�i�K�ڂ�
	if (hp_ <= changeHardHp_) {
		isBossHardMode_ = true;
		BossObject_->SetModel(hardBossModel_);
	}
	 
	 
}

void Boss::SetNormalAction(int action)
{

	this->normalAction_ = action;

	switch (action)
	{
	case BOSSNOTHING:
		shotType_ = BOSSNOTSHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSNORMAL:
		shotType_ = BOSSSTRAIGHTSHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHOMING:
		shotType_ = BOSSHOMINGSHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSMOVINGX:
		shotType_ = BOSSSTRAIGHTSHOT;
		moveType_ = BOSSMOVEX;
		break;

	case BOSSMOVINGY:
		shotType_ = BOSSSTRAIGHTSHOT;
		moveType_ = BOSSMOVEY;
		break;

	case BOSSMOVINGDIA:
		shotType_ = BOSSHOMINGSHOT;
		moveType_ = BOSSMOVEDIA;
		break;

	}
}

void Boss::SetHardAction(int action)
{
	this->normalAction_ = action;

	switch (action)
	{
	case BOSSHARDNOTHING:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHARDNORMAL:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHARDHOMING:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSNOTMOVE;
		break;

	case BOSSHARDMOVINGX:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSMOVEX;
		break;

	case BOSSHARDMOVINGY:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSMOVEY;
		break;

	case BOSSHARDMOVINGDIA:
		shotType_ = BOSSMULTISHOT;
		moveType_ = BOSSMOVEDIA;
		break;

	}
}

void Boss::ChangeAction()
{

	//�ʏ탂�[�h
	if (!isBossHardMode_) {
		int randNum = static_cast<int>(Random(0, normalActionSize_ - 0.001f));
		SetNormalAction(randNum);
	}
	else {
	//2�i�K�ڃ��[�h
		int randNum = static_cast<int>(Random(0, hardActionSize_ - 0.001f));
		SetHardAction(randNum);
	}
}

CollisionData Boss::GetBulletColData(int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	return it->get()->GetColData();
}

void Boss::SetBulletIsDead(bool isDead, int i)
{
	auto it = bullets_.begin();
	std::advance(it, i);

	it->get()->SetIsDead(isDead);
}

