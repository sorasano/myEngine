#include "SceneManager.h"

SceneManager::~SceneManager()
{
	//�Ō�̃V�[���̏I���Ɖ��
	scene_->Finalize();
	delete scene_;
}

void SceneManager::Update()
{
	//�V�[���؂�ւ��@�\

	//���V�[���̗\�񂪂���Ȃ�
	if (nextScene_) {
		//���V�[���̏I��
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		//�V�[���}�l�[�W���[���Z�b�g
		scene_->SetSceneManager(this);

		//�V�[���؂�ւ�
		scene_ = nextScene_;
		nextScene_ = nullptr;
		//���V�[��������������
		scene_->Initialize();
	}

	//���s���V�[�����X�V����
	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();
}

void SceneManager::DrawSprite()
{
	scene_->DrawSprite();
}
