/**
* @file ClearScene.h
* @brief�@�N���A�V�[��
*/


#pragma once
#include "BaseScene.h"

class ClearScene : public BaseScene
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

