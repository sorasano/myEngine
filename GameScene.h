#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Camera.h"
#include "object3D.h"
#include "Model.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "CSVLoader.h"

#include "Player.h"
#include "Enemy.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize(DirectXCommon* dxCommon, Input* input);

	void Update();

	void Draw();

	//�X�V�͈�
	bool UpadateRange(XMFLOAT3 cameraPos,XMFLOAT3 pos);

	void Collition();

private:
	//�f�o�C�X��input
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//�J����
	Camera* camera_{};

	//csv
	CSVLoader* enemyCsv = nullptr;

	//Fbx
	FbxModel* playerModel = nullptr;
	FbxModel* playerBulletModel = nullptr;
	FbxModel* enemyModel = nullptr;

	//�v���C���[
	std::unique_ptr<Player> player_;

	//�G
	std::list<std::unique_ptr<Enemy>> enemys_;
	size_t enemySize = 1000;


	//----�X�v���C�g----

	//�e�N�X�`��
	uint32_t test1Texture = 0;		
	Sprite* test1Sprite = nullptr;

	uint32_t test2Texture = 0;	
	Sprite* test2Sprite = nullptr;

	//�p�[�e�B�N��
	ParticleManager* particle1 = nullptr;
	ParticleManager* particle2 = nullptr;

	//�r���[�ϊ��s��
	XMFLOAT3 eye = { 0, 1, 30 };
	XMFLOAT3 target = { 0, 0, 0 };
	XMFLOAT3 up = { 0, 1, 0 };

};

