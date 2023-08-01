#include "GameScene.h"
#include "Random.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete camera_;
	delete testSprite;
	//for (int i = 0; i < enemySize; i++)
	//{
	//	FBX_SAFE_DELETE(enemyModel);
	//}
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

	//�����蔻��
	collisionManager_ = new Collision();

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

	//----------�w�i----------

	for (int i = 0; i < backGroundSize; i++) {
		std::unique_ptr<BackGround>newBackGround = std::make_unique<BackGround>();
		newBackGround->Initialize(adjustPos);

		//���݂̈ʒu+1���̃T�C�Y�Ŏ��̃}�b�v�̈ʒu�ɃZ�b�g
		adjustPos = newBackGround->GetPosition().z + newBackGround->GetSize();

		backGrounds_.push_back(std::move(newBackGround));
	}

	//�X�J�C�h�[��������
	Skydome* newSkydome = new Skydome();
	newSkydome->Initialize();
	skydome_.reset(newSkydome);

	//------------�v���C���[----------

	//�v���C���[������
	Player* newPlayer = new Player();
	newPlayer->Initialize(input_);
	player_.reset(newPlayer);

	//----------------�G--------------

	//���f�������w�肵�ăt�@�C���ǂݍ���
	enemyModel = FbxLoader::GetInstance()->LoadModelFromFile("enemy");
	enemyBulletModel = FbxLoader::GetInstance()->LoadModelFromFile("enemyBullet");

	//csv�t�@�C�����O
	enemyCsvsName_ = {
	"enemy1",
	"enemy2",
	"enemy3"
	};

	enemyCSVSize = static_cast<int>(enemyCsvsName_.size());

	//csv�f�[�^���t�@�C������,�z��ɓ���
	for (int i = 0; i < enemyCSVSize; i++) {

		//�t�@�C���̖��O���擾
		std::string fileName = enemyCsvsName_[i];

		std::unique_ptr<CSVLoader>newEnemyCsv = std::make_unique<CSVLoader>();
		newEnemyCsv->LoadCSV(fileName);
		enemyCsvs_.push_back(std::move(newEnemyCsv));

	}

	//csv�f�[�^�����ƂɂɓG�Ƀf�[�^���Z�b�g
	SetEnemy();
	
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
			enemy->Update(player_->GetPosition(), player_->GetSpeed());
		}
	}

	//�G�̃��X�g����폜�v���m�F
	CheckEnemy();

	//�w�i
	UpdateBackGround();

	//�����蔻��
	Collition();

	//�J�����X�V
	camera_->Update(player_->GetPosition());

	//�p�[�e�B�N���}�l�[�W���[�ÓI�X�V
	ParticleManager::StaticUpdate(camera_->GetEye(), camera_->GetTarget());

}

void GameScene::Draw()
{

	//�X�J�C�h�[��
	skydome_->Draw(dxCommon_->GetCommandList());

	//�w�i
	for (std::unique_ptr<BackGround>& backGround : backGrounds_) {
		if (UpadateRange(camera_->GetEye(), backGround->GetPosition())) {
			backGround->Draw(dxCommon_->GetCommandList());
		}
	}

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (UpadateRange(camera_->GetEye(), enemy->GetPosition())) {
			enemy->Draw(dxCommon_->GetCommandList());
		}
	}

	//���@
	player_->Draw(dxCommon_->GetCommandList());

}

void GameScene::DrawSprite()
{
	//�X�v���C�g
	testSprite->Draw(dxCommon_->GetCommandList());

	//���e�B�N��
	player_->DrawRaticle(dxCommon_->GetCommandList());
}

bool GameScene::UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos)
{

	if (cameraPos.x - pos.x < 20.0f && cameraPos.x - pos.x > -20.0f) { return true; }
	if (cameraPos.y - pos.y < 10.0f && cameraPos.y - pos.y > -10.0f) { return true; }
	if (cameraPos.z - pos.z < -10.0f && cameraPos.z - pos.z > -rangeMaxZ) { return true; }

	return false;
}

void GameScene::Collition()
{
#pragma region �G�Ǝ��@�̒e�̓����蔻��

	if (player_->GetBulletSize() != 0) {

		for (int i = 0; i < player_->GetBulletSize(); i++) {

			for (std::unique_ptr<Enemy>& enemy : enemys_)
			{
				if (!enemy->GetIsDead()) {
					//����������
					if (collisionManager_->CheckCollisionSquare(enemy->GetColData(), player_->GetBulletColData(i))) {

						//����������G�͏����ăp�[�e�B�N������
						enemy->SetISDesd(true);
						enemy->InitializeParticle();

						//���@�̒e�������A���@�̃X�s�[�h���グ�X�R�A�����Z
						player_->SetBulletIsDead(true, i);
						player_->SpeedUpByEnemy();
					}
				}
			}
		}
	}

#pragma endregion 

#pragma region	�G�Ǝ��@�̓����蔻��

	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (!enemy->GetIsDead()) {
			//����������
			if (collisionManager_->CheckCollisionSquare(enemy->GetColData(), player_->GetColData())) {

				//����������G�͏����ăp�[�e�B�N������
				enemy->SetISDesd(true);
				enemy->InitializeParticle();

				//���@�̃X�s�[�h������,�������G���Ԃ�
				player_->SpeedDownByEnemy();
				player_->SetIsInvincible(true);

			}
		}
	}

#pragma endregion 

#pragma region	�G�ƓG

	const int enemySize = static_cast<const int>(enemys_.size());

	for (int i = 0; i < enemySize; i++) {
		for (int j = 0; j < enemySize; j++) {

			if (i < j) {
				break;
			}

			auto it1 = enemys_.begin();
			std::advance(it1, i);

			auto it2 = enemys_.begin();
			std::advance(it2, j);

			if (!it1->get()->GetIsDead() && !it2->get()->GetIsDead()) {

				//����������
				if (collisionManager_->CheckCollisionSquare(it1->get()->GetColData(), it2->get()->GetColData())) {

					//���������甽�˂�����
					it1->get()->Reflection();
					it2->get()->Reflection();

				}

			}

		}
	}

#pragma endregion 

#pragma region ���@�ƓG�̒e

	for (std::unique_ptr<Enemy>& enemy : enemys_)
	{
		if (enemy->GetBulletSize() != 0) {

			for (int i = 0; i < enemy->GetBulletSize(); i++) {

				if (!enemy->GetIsDead()) {
					//����������
					if (collisionManager_->CheckCollisionSquare(player_->GetColData(), enemy->GetBulletColData(i))) {

						//����������G�̒e�������A���@�̃X�s�[�h���������X�R�A�����Z
						enemy->SetBulletIsDead(true, i);
						player_->SpeedDownByEnemy();

					}
				}
			}
		}
	}

#pragma endregion 

}


void GameScene::CheckEnemy()
{
	for (std::unique_ptr<Enemy>& enemy : enemys_) {

		//�J���������ɍs�����玀�S
		if (enemy->GetPosition().z < camera_->GetEye().z) {
			enemy->SetISDesd(true);
		}

	}

	//�f�X�t���O��true�Ńp�[�e�B�N�����o���Ȃ���΃��X�g����폜
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {return enemy->GetIsDead() && !enemy->GetIsParticle(); });

	//�G��0�ɂȂ�����
	if (enemys_.size() == 0) {

		SetEnemy();

	}

}

void GameScene::UpdateBackGround()
{

	//�w�i
	for (std::unique_ptr<BackGround>& backGround : backGrounds_)
	{
		backGround->Update();

		//�w�i�̈ʒu���J�������ʂ�߂�����
		if (backGround->GetPosition().z + backGround->GetSize() < camera_->GetEye().z) {

			//�߂����I�u�W�F�N�g���폜
			backGround->DeleteObject();

			//�I�u�W�F�N�g��z�u���Ȃ���
			backGround->SetObject(adjustPos);

			//���݂̈ʒu+1���̃T�C�Y�Ŏ��̃}�b�v�̈ʒu�ɃZ�b�g
			adjustPos = backGround->GetPosition().z + backGround->GetSize();

		}

	}

	//�X�J�C�h�[��
	skydome_->Update(camera_->GetEye().z);

}

void GameScene::SetEnemy()
{

	//����������ʒu�͍ő�`�拗������
	float makePos = player_->GetPosition().z + rangeMaxZ;

	//���Ԗڂ�CSV���Z�b�g���邩(�����_��)
	int setNum = static_cast<int>(Random(0,enemyCSVSize - 0.001f));
	auto it = enemyCsvs_.begin();
	std::advance(it, setNum);

	for (int i = 0; i < it->get()->GetSize(); i++)
	{
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize(enemyModel, enemyBulletModel);

		newObject->SetPosition(XMFLOAT3(it->get()->GetPosition(i).x, it->get()->GetPosition(i).y, it->get()->GetPosition(i).z + makePos));
		newObject->SetType(it->get()->GetType(i));
		newObject->SetStopInScreen(it->get()->GetStopInScreen(i));

		enemys_.push_back(std::move(newObject));
	}
}
