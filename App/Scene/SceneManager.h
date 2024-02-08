/**
* @file TitleScene.h
* @brief�@�V�[���̐؂�ւ��A���s
*/

#pragma once
#include "BaseScene.h"

class SceneManager
{
public:
	/**
	* �f�X�g���N�^
	*/
	~SceneManager();

	/**
	* ���V�[���\��
	*/
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }

	/**
	* �X�V
	*/
	void Update();
	/**
	* �`��
	*/
	void Draw();
	/**
	* �X�v���C�g�`��
	*/
	void DrawSprite();

private:
	//���̃V�[��(���s���V�[��)
	BaseScene* scene_ = nullptr;
	//���̃V�[��
	BaseScene* nextScene_ = nullptr;
};

