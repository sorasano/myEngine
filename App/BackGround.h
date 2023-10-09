#pragma once
#include "JsonLoader.h"

#include "FbxLoader.h"
#include "FbxObject3d.h"

#include <map>

class BackGround
{
public:
	//num = ���Ԃ̔z�u�f�[�^��ǂݍ��݂��ނ�,adjustPos = �ǂ̂��炢���炷��
	void Initialize(float adjustPos_);

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList_);

	//�I�u�W�F�N�g�̔z�u
	void SetObject(float adjustPos_);
	//�z�u�f�[�^�����[�h num = ���Ԃ̔z�u�f�[�^��ǂݍ��݂��ނ�
	void LoadJson(int num);
	//�I�u�W�F�N�g�̔z�u adjustPos = �ǂ̂��炢���炷��
	void ArrangementObject(float adjustPos_);

	//�I�u�W�F�N�g�̍폜
	void DeleteObject();

	//�Q�b�^�[
	float GetSize() { return size; }
	XMFLOAT3 GetPosition() { return position_; }

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
	float size = 520.0f;
	//�w�i�̒��S�̈ʒu(ground�̈ʒu)
	XMFLOAT3 position_ = {};
};

