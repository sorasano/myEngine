/**
* @file BossScene.h
* @brief�@�{�X�V�[��
*/

#pragma once
#include "BaseScene.h"

class BossScene : public BaseScene
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

