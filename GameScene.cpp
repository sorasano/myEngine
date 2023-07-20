#include "GameScene.h"
#include "Random.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete testSprite;
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


	//�J����
	//�J����������
	camera_ = new Camera;
	camera_->StaticInitialize(dxCommon->GetDevice());
	camera_->Initialize(input_);

	//�`�揉���������@��������

	// �p�[�e�B�N���ÓI������
	ParticleManager::StaticInitialize(dxCommon, WinApp::winW, WinApp::winH);


	//----------FBX----------

	//fbxLoadr�ėp������
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());

	//�f�o�C�X���Z�b�g
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera_);
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3D::CreateGraphicsPipeline();

	//���f�������w�肵�ăt�@�C���ǂݍ���
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("player");
	playerBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("playerBullet");
	enemyModel = FbxLoader::GetInstance()->LoadModelFromFile("enemy");
	bReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("backReticle");
	fReticleModel = FbxLoader::GetInstance()->LoadModelFromFile("frontReticle");

	//�w�i
	for (int i = 0; i < backGroundSize; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(backGroundNum,adjustPos);

		backGroundNum++;
		adjustPos += (backGroundNum * newBackGround->GetSize());

		backGrounds_.push_back(std::move(newBackGround));

	}

	//------------�v���C���[----------

	//�v���C���[������
	Player* newPlayer = new Player();
	newPlayer->Initialize(input_, playerModel, playerBulletModel, fReticleModel, bReticleModel);
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
		XMFLOAT3 md_pos = { 10.0f,5.0f,1.5f };
		XMFLOAT3 pos{};
		pos.x = Random(-md_pos.x, md_pos.x);
		pos.y = Random(-md_pos.y, md_pos.y);
		pos.z = Random(-md_pos.z, md_pos.z);

		pos.z = (oldPos + lowInterval);
		oldPos = pos.z;

		newObject->SetPosition(pos);
		enemys_.push_back(std::move(newObject));

	}

	//�X�v���C�g�}�l�[�W���[
	SpriteManager::SetDevice(dxCommon->GetDevice());
	spriteManager = new SpriteManager;
	spriteManager->Initialize();

	//------�e�N�X�`��------
	spriteManager->LoadFile(0, L"Resources/reimu.png");

	//-----�X�v���C�g------
	Sprite::SetDevice(dxCommon->GetDevice());
	Sprite::SetSpriteManager(spriteManager);
	Sprite::CreateGraphicsPipeLine();

	testSprite = new Sprite();
	testSprite->SetTextureNum(0);
	testSprite->Initialize();
	//�A���J�[�|�C���g���X�v���C�g�̒��S��
	testSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	testSprite->SetScale(XMFLOAT2(600, 300));
	testSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	testSprite->Update();
}

void GameScene::Update()
{
	//���@
	player_->Update();

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Update();
		}
	}

	//�w�i
	for (std::unique_ptr<BackGround>& backGround : backGrounds_)
	{
		backGround->Update();
	}

	//�����蔻��
	Collition();

	//�J�����X�V
	camera_->Update(player_->GetPosition());

	//�p�[�e�B�N���}�l�[�W���[�ÓI�X�V
	ParticleManager::StaticUpdate(camera_->GetEye(), camera_->GetTarget());

}

void GameScene::Draw()
{

	//���@
	player_->Draw(dxCommon_->GetCommandList());

	//�w�i
	for (std::unique_ptr<BackGround>& backGround : backGrounds_) {
		backGround->Draw(dxCommon_->GetCommandList());
	}

	//�X�v���C�g
	testSprite->Draw(dxCommon_->GetCommandList());

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}

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

void GameScene::CheckEnemy()
{
	for (std::unique_ptr<Enemy>& enemy : enemys_) {

		//�f�X�t���O
		if (enemy->GetIsDead()) {
			enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->GetIsDead(); });
		}
	}
}
