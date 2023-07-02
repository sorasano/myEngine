#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete test1Sprite;
	delete test2Sprite;
	delete particle1;
	delete particle2;
	FBX_SAFE_DELETE(playerModel);
	FBX_SAFE_DELETE(playerBulletModel);
	for (int i = 0; i < enemySize; i++)
	{
		FBX_SAFE_DELETE(enemyModel);
	}
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input_)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input_;

	//�`�揉���������@��������

	//�J����
	//�J����������
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon->GetDevice());
	camera_->Initialize(eye, target, up, input_);

	//�f�o�C�X���Z�b�g
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera_);
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3D::CreateGraphicsPipeline();

	//----------FBX----------
	//���f�������w�肵�ăt�@�C���ǂݍ���
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	enemyModel = FbxLoader::GetInstance()->LoadModelFromFile("enemy");

	//------------�v���C���[----------

	//�v���C���[������
	Player* newPlayer = new Player();
	newPlayer->Initialize(input_, playerModel, playerBulletModel);
	player_.reset(newPlayer);

	//------------�G----------
	enemyCsv = new CSVLoader;
	enemyCsv->LoadCSV("Resources/csv/enemy.csv");

	float lowInterval = 50.0f;
	float oldPos = 0.0f;

	for (int i = 0; i < enemySize; i++)
	{
		//CSV
		//std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		//newObject->Initialize(enemyModel);

		//newObject->SetPosition(enemyCsv->GetPosition(i));
		//newObject->SetScale(enemyCsv->GetScale(i));
		//newObject->SetRotation(enemyCsv->GetRotation(i));

		//enemys_.push_back(std::move(newObject));

		//RAND
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(enemyModel);

		//�����_���ɕ��z
		XMFLOAT3 md_pos = { 20.0f,10.0f,3.0f };
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos.x - md_pos.x / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos.y - md_pos.y / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos.z - md_pos.z / 2.0f;

		pos.z = (oldPos + lowInterval);
		oldPos = pos.z;

		newObject->SetPosition(pos);
		enemys_.push_back(std::move(newObject));

	}

	//�X�v���C�g����
	//---test1---
	test1Texture = Texture::LoadTexture(L"Resources/reimu.png");

	test1Sprite = new Sprite();
	test1Sprite->Initialize(test1Texture);
	//�A���J�[�|�C���g���X�v���C�g�̒��S��
	test1Sprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	test1Sprite->SetPos(XMFLOAT2(WinApp::winW / 2, WinApp::winH / 2));
	test1Sprite->Update();

	//---test2---
	test2Texture = Texture::LoadTexture(L"Resources/texture.jpg");

	test2Sprite = new Sprite();
	test2Sprite->Initialize(test2Texture);
	//�A���J�[�|�C���g���X�v���C�g�̒��S��
	test2Sprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	test2Sprite->SetPos(XMFLOAT2(WinApp::winW / 2, WinApp::winH / 2 - 200));
	test2Sprite->Update();

	//�p�[�e�B�N��1

	// �p�[�e�B�N���ÓI������
	ParticleManager::StaticInitialize(dxCommon, WinApp::winW, WinApp::winH);

	particle1 = new ParticleManager();
	//�p�[�e�B�N������
	particle1->Initialize("Resources/effect1.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Z���ׂ�[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float md_pos = 10.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		//X,Y,Z���ׂ�[-0.05f,+0.05f]�Ń����_���ɕ��z
		const float md_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
		XMFLOAT3 acc{};
		const float md_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		//�ǉ�
		particle1->Add(600, pos, vel, acc);

	}

	particle1->Update();

	particle2 = new ParticleManager();
	//�p�[�e�B�N������
	particle2->Initialize("Resources/effect2.png");

	for (int i = 0; i < 100; i++) {
		//X,Y,Z���ׂ�[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float md_pos = 10.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		//X,Y,Z���ׂ�[-0.05f,+0.05f]�Ń����_���ɕ��z
		const float md_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
		XMFLOAT3 acc{};
		const float md_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		//�ǉ�
		particle2->Add(600, pos, vel, acc);

	}

	particle2->Update();
}

void GameScene::Update()
{
	//���@
	player_->Update();

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		enemy->Update();
	}

	//�����蔻��
	Collition();

	//----�p�[�e�B�N��----
	particle1->Update();
	particle2->Update();

	//�J�����X�V
	camera_->Update(player_->GetPosition());

}

void GameScene::Draw()
{

	////-------�w�i�X�v���C�g�`�揈��-------//
	SpriteManager::GetInstance()->beginDraw();

	//���@
	player_->Draw(dxCommon_->GetCommandList());
	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}


	////-------�O�i�X�v���C�g�`�揈��-------//
	SpriteManager::GetInstance()->beginDraw();

	//----�p�[�e�B�N��----
	//particle1->Draw();
	//particle2->Draw();

	//�X�v���C�g
	//test1Sprite->Draw();
	//test2Sprite->Draw();

}

bool GameScene::UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -500.0f) { return true; }

	return false;
}

void GameScene::Collition()
{
	//�G�Ǝ��@�̒e�̓����蔻��
	if (player_->GetBulletSize() != 0) {

		for (int i = 0; i < player_->GetBulletSize(); i++) {

			for (std::unique_ptr<Enemy>& enemy : enemys_)
			{
				if (!enemy->GetIsDead()) {
					//����������
					if (enemy->Collition(player_->GetBulletPosition(i), player_->GetBulletColSize(i))) {

						//���������玩�@�̒e�������A���@�̃X�s�[�h���グ�X�R�A�����Z
						player_->SetBulletIsDead(true, i);
						player_->SpeedUpByEnemy();

					}
				}
			}
		}
	}

	//�G�Ǝ��@�̓����蔻��
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (!enemy->GetIsDead()) {
			//����������
			if (enemy->Collition(player_->GetPosition(), player_->GetColSize())) {

				//���������玩�@�̃X�s�[�h������,�������G���Ԃ�
				player_->SpeedDownByEnemy();
				player_->SetIsInvincible(true);
			}
		}
	}

}
