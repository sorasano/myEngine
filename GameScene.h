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

#include "Skydome.h"
#include "BackGround.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"

#include <map>

enum Scene {
	TITLE,
	NORMALPLAY,
	BOSSPLAY,
	CLEAR
};

class GameScene
{
public:
	GameScene();
	~GameScene();

	void Initialize(DirectXCommon* dxCommon, Input* input);

	void Update();

	void Draw();

	void DrawSprite();

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

	//���Z�b�g
	void Reset();

	//�{�X��
	//�������o,������
	void BossSceneInitialize();
	//���j���o
	void BossSceneFinalize();

	void BossSceneCollition();


private:
	//�f�o�C�X��input
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//�J����
	Camera* camera_{};

	//�V�[��
	int scene = TITLE;

	//�t�F�[�Y
	int phase = 0;
	const int MaxPhase = 1;

	//Fbx
	FbxModel* enemyModel = nullptr;
	FbxModel* enemyBulletModel = nullptr;

	//�X�J�C�h�[��
	std::unique_ptr<Skydome> skydome_;

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

	//�{�X
	std::unique_ptr<Boss> boss_;

	//�X�v���C�g�}�l�[�W���[
	SpriteManager* spriteManager = nullptr;
	//�X�v���C�g
	Sprite* titleSprite = nullptr;
	Sprite* clearSprite = nullptr;


	//�����蔻��
	Collision* collisionManager_ = nullptr;

	//�`��ő勗��
	float rangeMaxZ = 500.0f;
};

