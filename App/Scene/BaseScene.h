#pragma once
/**
* @file BaseScene.h
* @brief �V�[���̊��N���X
*/

//�O���錾
class SceneManager;

class BaseScene {

public:
	/**
	* �f�X�g���N�^
	*/
	virtual ~BaseScene() = default;

	/**
	* ������
	*
	*/
	virtual void Initialize() = 0;
	/**
	* �X�V
	*/
	virtual void Update() = 0;
	/**
	* �`��
	*/
	virtual void Draw() = 0;
	/**
	* �X�v���C�g�`��
	*/
	virtual void DrawSprite() = 0;
	/**
	* �I������
	*/
	virtual void Finalize() = 0;

	//�Z�b�^�[

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:
	//�V�[���}�l�[�W��(�؂�Ă���)
	SceneManager* sceneManager_ = nullptr;


};

