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
	CLEAR,
	GAMEOVER
};

enum Performance {
	TITLETOPLAY,//�^�C�g�����v���C�V�[���J��
	INBOSS,//�{�X�퓱��
	CLEARBOSS,//�{�X��N���A
	GAMEOVERBOSS//�{�X��Q�[���I�[�o�[
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

	//�V�[���؂�ւ�
	void ChangeScene();

	//�v���C�V�[��������
	void PlaySceneInitialize();

	//�{�X��
	//������
	void BossSceneInitialize();
	//�����蔻��
	void BossSceneCollition();

	//���o
	void UpdatePerformance();
	//�^�C�g�����v���C�V�[���J�ډ��o
	void TitleToPlayPerformance();
	//�{�X�퓱�����o
	void BossInPerformance();
	//�{�X���j���o
	void BossClearPerformance();
	//�{�X���S���o
	void BossGameoverPerformance();

private:
	//�f�o�C�X��input
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	//�J����
	Camera* camera_{};

	//�V�[��
	int scene_ = TITLE;

	//�t�F�[�Y
	int phase_ = 0;
	const int MaxPhase_ = 3;

	//Fbx
	FbxModel* enemyModel_ = nullptr;
	FbxModel* enemyBulletModel_ = nullptr;

	//�X�J�C�h�[��
	std::unique_ptr<Skydome> skydome_;

	//�w�i
	std::list<std::unique_ptr<BackGround>> backGrounds_;
	size_t backGroundSize_ = 5;
	//�����p�̍��W
	float adjustPos_ = 0;

	//�v���C���[
	std::unique_ptr<Player> player_;

	//�G
	std::list<std::unique_ptr<Enemy>> enemys_;

	//csv
	std::list<std::unique_ptr<CSVLoader>> enemyCsvs_;
	std::vector<std::string> enemyCsvsName_;
	int enemyCSVSize_ = 0;

	//�{�X
	std::unique_ptr<Boss> boss_;

	//�X�v���C�g�}�l�[�W���[
	SpriteManager* spriteManager_ = nullptr;
	//�X�v���C�g
	Sprite* titleSprite_ = nullptr;
	Sprite* clearSprite_ = nullptr;

	//�����蔻��
	Collision* collisionManager_ = nullptr;

	//���o�t���O
	bool isPerformance_ = false;
	//�Ȃ�̉��o��
	int performanceNum_ = TITLETOPLAY;
};

