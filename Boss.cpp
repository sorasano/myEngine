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
	FBX_SAFE_DELETE(BossObject);
}

void Boss::Initialize()
{
	//���f�������w�肵�ăt�@�C���ǂݍ���
	bossModel = FbxLoader::GetInstance()->LoadModelFromFile("boss");
	bossBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("bossBullet");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	BossObject = new FbxObject3D;
	BossObject->Initialize();
	BossObject->SetModel(bossModel);

	this->bulletModel_ = bossBulletModel;

}

void Boss::Update(XMFLOAT3 pPos, float pSpeed)
{
	//�v���C���[���X�V
	this->playerPosition_ = pPos;
	this->playerSpeed_ = pSpeed;

	if (!isDead) {

		//�s���ω�
		actionCollTimer++;
		if (actionCollTimer > ActionCoolTime) {
			actionCollTimer = 0;
			ChangeAction();
		}

		//�ړ�
		Move();
		
		//�ˌ�
		if (shotType != BOSSNOTSHOT) {
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

	BossObject->SetPosition(position_);
	BossObject->SetScale(scale_);
	BossObject->SetRotate(rotation_);
	BossObject->Update();
}

void Boss::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead) {
		BossObject->Draw(cmdList);

		//�e
		for (std::unique_ptr<BossBullet>& bullet : bullets_)
		{
			bullet->Draw(cmdList);
		}
	}
	//----�p�[�e�B�N��----
	else if (isParticle) {
		particle->Draw();
	}

}

void Boss::Move()
{
	//���@�ɒǏ]
	position_.z += playerSpeed_;

	switch (moveType)
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

void Boss::MoveY()
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


void Boss::InitializeParticle()
{
	//�t���O��true��
	isParticle = true;
	//�^�C�}�[�Z�b�g
	particleTimer = ParticleTime;

	//�p�[�e�B�N������
	particle = new ParticleManager();
	particle->Initialize("Resources/effect/effect1.png");

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

void Boss::UpdateParticle()
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

void Boss::Shot()
{
	bulletCoolTimer++;

	if (BulletCoolTime < bulletCoolTimer) {
		MakeBullet();

		bulletCoolTimer = 0;
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

void Boss::MakeBullet()
{
	Vector3 velocity = {};

	switch (shotType)
	{
	case BOSSNOTSHOT:

		break;

	case BOSSSTRAIGHTSHOT:
		//z����-�����̒P�ʃx�N�g���ɑ��x��������
		velocity = { 0.0f,0.0f,-1.0f };
		velocity *= bulletSpeed;
		break;

	case BOSSHOMINGSHOT:

		//���@�ƓG�̃x�N�g�������
		Vector3 playerVec = { playerPosition_.x ,playerPosition_.y,playerPosition_.z };
		Vector3 BossVec = { position_.x,position_.y,position_.z };

		velocity = playerVec - BossVec;

		//���K�������đ��x��������
		velocity.normalize();
		velocity *= bulletSpeed;

		velocity.z += playerSpeed_;

		break;
	}

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
	isDead = false;

	particleTimer = 0;
}

CollisionData Boss::GetColData()
{

	CollisionData colData;

	colData.position = this->position_;
	colData.size = this->colSize_;

	return colData;
}

void Boss::SetAction(int action)
{

	this->action = action;

	switch (action)
	{
	case BOSSNOTHING:
		shotType = BOSSNOTSHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSNORMAL:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSHOMING:
		shotType = BOSSHOMINGSHOT;
		moveType = BOSSNOTMOVE;
		break;

	case BOSSMOVINGX:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSMOVEX;
		break;

	case BOSSMOVINGY:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSMOVEY;
		break;

	case BOSSMOVINGDIA:
		shotType = BOSSSTRAIGHTSHOT;
		moveType = BOSSMOVEDIA;
		break;
	}
}

void Boss::ChangeAction()
{
	int randNum = static_cast<int>(Random(0, actionSize - 0.001f));

	SetAction(randNum);
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

