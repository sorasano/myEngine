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
	FBX_SAFE_DELETE(fbxModel1);
	FBX_SAFE_DELETE(fbxObject1);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input_)
{

	this->dxCommon_ = dxCommon;
	this->input_ = input_;

	//�`�揉���������@��������
	HRESULT result;

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
	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("cube");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	fbxObject1 = new FbxObject3D;
	fbxObject1->Initialize();
	fbxObject1->SetModel(fbxModel1);

	//���f�������w�肵�ăt�@�C���ǂݍ���
	fbxModel2 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//3d�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	fbxObject2 = new FbxObject3D;
	fbxObject2->Initialize();
	fbxObject2->SetModel(fbxModel2);
	fbxObject2->PlayAnimation();

	//3d���f��
	//----------��----------
	//���I�u�W�F�N�g�̃��f��������
	Model* newSphereModel = new Model();
	newSphereModel->Initialize(dxCommon, "sphere", "Resources/sphere/white1�~1.png");
	sphereModel_.reset(newSphereModel);

	//���I�u�W�F�N�g������
	Sphere* newSphere = new Sphere();
	newSphere->Initialize(dxCommon, sphereModel_.get());
	sphere_.reset(newSphere);

	//���I�u�W�F�N�g�̃��f��������
	Model* newSphereRedModel = new Model();
	newSphereRedModel->Initialize(dxCommon, "sphere", "Resources/sphere/red1x1.png");
	sphereRedModel_.reset(newSphereRedModel);

	//���I�u�W�F�N�g������
	Sphere* newRedSphere = new Sphere();
	newRedSphere->Initialize(dxCommon, sphereRedModel_.get());
	sphereRed_.reset(newRedSphere);

	//---------�n��-----------

	////�n�ʃI�u�W�F�N�g
	//std::unique_ptr<Model> planeModel_;
	//std::unique_ptr<Plane> plane_;

	////�n�ʃI�u�W�F�N�g�̃��f��������
	//Model* newPlaneModel = new Model();
	//newPlaneModel->Initialize(dxCommon, "plane", "Resources/plane/white1�~1.png");
	//planeModel_.reset(newPlaneModel);

	////�n�ʃI�u�W�F�N�g������
	//Plane* newPlane = new Plane();
	//newPlane->Initialize(dxCommon,planeModel_.get());
	//plane_.reset(newPlane);


	//------------�v���C���[----------

	//�v���C���[�̃��f��
	//std::unique_ptr<Model> playerModel_;

	////�v���C���[
	//std::unique_ptr<Player> player_;

	////�v���C���[�̃��f��������
	//Model* newModel = new Model();
	//newModel->Initialize(dxCommon, "Player", "Resources/Player/blue.png");
	//playerModel_.reset(newModel);

	////�v���C���[������
	//Player* newPlayer = new Player();
	//newPlayer->Initialize(dxCommon, playerModel_.get());
	//player_.reset(newPlayer);

	//�����蔻��

	////��
	sphere_->sphereCol.radius = 1;
	sphere_->sphereCol.center = XMVECTOR{ sphere_->GetPosition().x, sphere_->GetPosition().y, sphere_->GetPosition().z,1 };

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

	sphere_->setPosition(position_);
	sphere_->setRotation(XMFLOAT3(0, 0, 0));
	/*	sphere_->setScale(XMFLOAT3(1, 100, 100));*/

	sphere_->Update(camera->matView, camera->matProjection);

	sphereRed_->setPosition(XMFLOAT3(position_.x + 3, position_.y, position_.z));
	sphereRed_->Update(camera->matView, camera->matProjection);

	//plane_->Update(matView, matProjection);

	sphere_->sphereCol.center = XMVECTOR{ sphere_->GetPosition().x, sphere_->GetPosition().y, sphere_->GetPosition().z,1 };

	//hit = Collision::CheckSphere2Plane(sphere_->sphereCol,plane_->planeCol);


	//fbx
	fbxObject1->Update();
	fbxObject2->Update();

	//----�p�[�e�B�N��----
	particle1->Update();
	particle2->Update();

}

void GameScene::Draw()
{

	////-------�w�i�X�v���C�g�`�揈��-------//
	SpriteManager::GetInstance()->beginDraw();

	//sphere_->Draw();
	//sphereRed_->Draw();

	//fbxObject1->Draw(dxCommon_->GetCommandList());
	fbxObject2->Draw(dxCommon_->GetCommandList());


	////-------�O�i�X�v���C�g�`�揈��-------//
	SpriteManager::GetInstance()->beginDraw();

	//----�p�[�e�B�N��----
	//particle1->Draw();
	//particle2->Draw();

	//�X�v���C�g
	//test1Sprite->Draw();
	//test2Sprite->Draw();

}
