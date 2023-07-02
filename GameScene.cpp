#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete test1Sprite;
	delete test2Sprite;
	delete camera;
	delete particle1;
	delete particle2;
	FBX_SAFE_DELETE(playerModel);
	FBX_SAFE_DELETE(fbxModel2);
	FBX_SAFE_DELETE(fbxObject2);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input_)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input_;

	//�`�揉���������@��������

	//�J����
	//�J����������
	camera = new Camera;
	camera->StaticInitialize(dxCommon->GetDevice());
	camera->Initialize(eye, target, up, input_);

	camera->SetTarget({0,0,0});
	camera->SetEye({ 0,0,30 });

	//�f�o�C�X���Z�b�g
	FbxObject3D::SetDevice(dxCommon->GetDevice());
	FbxObject3D::SetCamera(camera);
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3D::CreateGraphicsPipeline();

	//���f�������w�肵�ăt�@�C���ǂݍ���
	playerModel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	fbxModel2 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	fbxObject2 = new FbxObject3D;
	fbxObject2->Initialize();
	fbxObject2->SetModel(fbxModel2);
	fbxObject2->PlayAnimation();

	//------------�v���C���[----------

	//�v���C���[������
	Player* newPlayer = new Player();
	newPlayer->Initialize(playerModel);
	player_.reset(newPlayer);

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
	//�ˉe�ϊ�

	//�J�����X�V
	camera->Update();

	//----��----

	//���s�ړ��X�V

	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//���W���ړ����鏈��(Z���W)
		if (input_->PushKey(DIK_UP)) { position_.y += 0.1f; }
		else if (input_->PushKey(DIK_DOWN)) { position_.y -= 0.1f; }
		if (input_->PushKey(DIK_RIGHT)) { position_.x -= 0.1f; }
		else if (input_->PushKey(DIK_LEFT)) { position_.x += 0.1f; }

	}

	//fbx
	player_->Update();
	fbxObject2->Update();

	//----�p�[�e�B�N��----
	particle1->Update();
	particle2->Update();

}

void GameScene::Draw()
{

	////-------�w�i�X�v���C�g�`�揈��-------//
	SpriteManager::GetInstance()->beginDraw();

	player_->Draw(dxCommon_->GetCommandList());
	////fbxObject2->Draw(dxCommon_->GetCommandList());


	////-------�O�i�X�v���C�g�`�揈��-------//
	SpriteManager::GetInstance()->beginDraw();

	//----�p�[�e�B�N��----
	//particle1->Draw();
	//particle2->Draw();

	//�X�v���C�g
	//test1Sprite->Draw();
	//test2Sprite->Draw();

}
