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
#include "Collision.h"

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
	bool UpadateRange(XMFLOAT3 cameraPos, XMFLOAT3 pos);

	//�����蔻��
	void Collition();

	//�G�����m�F
	void CheckEnemy();

	//�w�i�X�V
	void UpdateBackGround();

	//�G�z�u
	void SetEnemy();

private:
	//�f�o�C�X��input
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//�J����
	Camera* camera_{};

	//Fbx
	FbxModel* enemyModel = nullptr;
	FbxModel* enemyBulletModel = nullptr;

	//�w�i
	std::list<std::unique_ptr<BackGround>> backGrounds_;
	size_t backGroundSize = 4;
	//�����p�̍��W
	float adjustPos = 0;

	//�v���C���[
	std::unique_ptr<Player> player_;

	//�G
	std::list<std::unique_ptr<Enemy>> enemys_;

	//csv
	std::list<std::unique_ptr<CSVLoader>> enemyCsvs_;
	std::vector<std::string> enemyCsvsName_;
	int enemyCSVSize = 0;

	//�X�v���C�g�}�l�[�W���[
	SpriteManager* spriteManager = nullptr;
	//�e�N�X�`��
	Sprite* testSprite = nullptr;

	//�����蔻��
	Collision* collisionManager_ = nullptr;

	//�`��ő勗��
	float rangeMaxZ = 500.0f;
};

