/**
* @file GameoverScene.h
* @brief�@�Q�[���I�[�o�[�V�[��
*/

#pragma once
#include "BaseScene.h"

class GameoverScene : public BaseScene
{
public:
	/**
	* ������
	*
	*/
	void Initialize() override;
	/**
	* �X�V
	*/
	void Update() override;
	/**
	* �`��
	*/
	void Draw() override;
	/**
	* �X�v���C�g�`��
	*/
	void DrawSprite() override;
	/**
	* �I������
	*/
	void Finalize() override;
};

