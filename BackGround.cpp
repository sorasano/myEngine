#include "BackGround.h"

#include "Random.h"
#define PI 3.1415

void BackGround::Initialize(float adjustPos)
{
	//���f���ǂݍ���
	groundModel = FbxLoader::GetInstance()->LoadModelFromFile("ground");
	boxModel = FbxLoader::GetInstance()->LoadModelFromFile("box");
	treeModel = FbxLoader::GetInstance()->LoadModelFromFile("tree");
	wallModel = FbxLoader::GetInstance()->LoadModelFromFile("wall");
	pillarModel = FbxLoader::GetInstance()->LoadModelFromFile("pillar");

	backGroundModels.insert(std::make_pair("ground", groundModel));
	backGroundModels.insert(std::make_pair("box", boxModel));
	backGroundModels.insert(std::make_pair("tree", treeModel));
	backGroundModels.insert(std::make_pair("wall", wallModel));
	backGroundModels.insert(std::make_pair("pillar", pillarModel));

	//-------�w�i�z�u------
	SetObject(adjustPos);
}

void BackGround::Update()
{
	for (auto& object : backGroundObjects) {
		object->Update();
	}
}

void BackGround::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& object : backGroundObjects) {
		object->Draw(cmdList);
	}
}

void BackGround::SetObject(float adjustPos)
{
	//�����_���Ŕz�u����}�b�v��I��
	int num = static_cast<int>(Random(1.0f,4.99f));

	//json�t�@�C���ǂݍ���
	LoadJson(num);
	//�I�u�W�F�N�g�̔z�u
	ArrangementObject(adjustPos);
}

void BackGround::LoadJson(int num)
{
	// ���x���f�[�^�̓ǂݍ���
	switch (num)
	{
	case 1:
		levelData = JsonLoader::LoadFile("backGround1");
		break;
	case 2:
		levelData = JsonLoader::LoadFile("backGround2");
		break;
	case 3:
		levelData = JsonLoader::LoadFile("backGround3");
		break;
	case 4:
		levelData = JsonLoader::LoadFile("backGround4");
		break;
	}
}

void BackGround::ArrangementObject(float adjustPos)
{

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData->objects) {
		// �t�@�C��������o�^�ς݃��f��������
		FbxModel* model = nullptr;
		decltype(backGroundModels)::iterator it = backGroundModels.find(objectData.fileName);
		if (it != backGroundModels.end()) {
			model = it->second;
		}

		// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		FbxObject3D* newObject = new FbxObject3D;
		newObject->Initialize();
		newObject->SetModel(model);

		// ���W
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, objectData.translation);
		//���ɔz�u
		pos.y -= 12.0f;
		//����
		pos.z += adjustPos;
		newObject->SetPosition(pos);

		// ��]�p
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, objectData.rotation);
		rot.x = static_cast<float>(90 * (PI / 180));
		newObject->SetRotate(rot);

		// �T�C�Y
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, objectData.scaling);
		newObject->SetScale(scale);

		//�X�V
		newObject->Update();

		// �z��ɓo�^
		backGroundObjects.push_back(newObject);

		//�I�u�W�F�N�g��ground�̎����̃|�W�V�����𒆐S�Ƃ���
		if (it->first == "ground") {
			this->position = pos;
		}
	}
}

void BackGround::DeleteObject()
{
	for (auto& object : backGroundObjects) {
		//�v�f�̉��
		delete object;
	}
	//���ׂĂ̗v�f���R���e�i�����菜��
	backGroundObjects.clear();
}
