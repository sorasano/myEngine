#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Camera.h"
#include "object3D.h"
#include "Model.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "Collision.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"

#include "Player.h"
#include "Sphere.h"
#include "Plane.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize(DirectXCommon* dxCommon, Input* input);

	void Update();

	void Draw();

private:
	//�f�o�C�X��input
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	//�J����
	std::unique_ptr<Camera> camera_;

	ID3D12GraphicsCommandList* commandList = nullptr;

	//�I�u�W�F�N�g
	std::unique_ptr<Object3D> object3Ds_;
	//���I�u�W�F�N�g
	std::unique_ptr<Model> sphereModel_;
	std::unique_ptr<Sphere> sphere_;

	std::unique_ptr<Model> sphereRedModel_;
	std::unique_ptr<Sphere> sphereRed_;

	//Fbx
	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObject1 = nullptr;

	FbxModel* fbxModel2 = nullptr;
	FbxObject3D* fbxObject2 = nullptr;

	//�J����������
	Camera* camera{};

	//----�X�v���C�g----

	//�e�N�X�`��
	uint32_t test1Texture = 0;		
	Sprite* test1Sprite = nullptr;

	uint32_t test2Texture = 0;	
	Sprite* test2Sprite = nullptr;

	//�p�[�e�B�N��
	ParticleManager* particle1 = nullptr;
	ParticleManager* particle2 = nullptr;

	//�A�t�B���ϊ����
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,1,0 };

	//�r���[�ϊ��s��

	XMFLOAT3 eye = { -10, 1, 50 };
	XMFLOAT3 target = { 0, 0, 0 };
	XMFLOAT3 up = { 0, 1, 0 };

	//�����蔻��
	bool hit = false;

};

