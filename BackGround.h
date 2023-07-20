#pragma once
#include "JsonLoader.h"

#include "FbxLoader.h"
#include "FbxObject3d.h"

#include <map>

class BackGround
{
public:
	//num = ���Ԃ̔z�u�f�[�^��ǂݍ��݂��ނ�,adjustPos = �ǂ̂��炢���炷��
	void Initialize(int num,float adjustPos);

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�z�u�f�[�^�����[�h num = ���Ԃ̔z�u�f�[�^��ǂݍ��݂��ނ�
	void LoadJson(int num);
	//�I�u�W�F�N�g�̔z�u adjustPos = �ǂ̂��炢���炷��
	void ArrangementObject(float adjustPos);

	//�Q�b�^�[
	float GetSize() { return size; }

private:

	//fbx���f��
	FbxModel* groundModel = nullptr;
	FbxModel* boxModel = nullptr;
	FbxModel* treeModel = nullptr;
	FbxModel* wallModel = nullptr;
	FbxModel* pillarModel = nullptr;

	//���f���z��
	std::map<std::string, FbxModel*> backGroundModels;
	std::vector<FbxObject3D*> backGroundObjects;

	//�z�u�f�[�^
	LevelData* levelData = nullptr;

	//�S�̂̃T�C�Y
	float size = 180.0f;
};

