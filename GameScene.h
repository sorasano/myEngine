#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "Camera.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "CSVLoader.h"
#include "JsonLoader.h"

#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"

#include <map>

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

	//�G�����m�F
	void CheckEnemy();

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
	FbxModel* fReticleModel = nullptr;
	FbxModel* bReticleModel = nullptr;

	//�w�i
	std::list<std::unique_ptr<BackGround>> backGrounds_;
	size_t backGroundSize = 4;
	//�����ڂ̔w�i��
	int backGroundNum = 1;
	float adjustPos = 0;

	//�v���C���[
	std::unique_ptr<Player> player_;

	//�G
	std::list<std::unique_ptr<Enemy>> enemys_;
	size_t enemySize = 100;


	//----�X�v���C�g----

	//�X�v���C�g�}�l�[�W���[
	SpriteManager* spriteManager = nullptr;

	//�e�N�X�`��
	Sprite* testSprite = nullptr;

};

