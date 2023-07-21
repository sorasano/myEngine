#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

// ���x���f�[�^
struct LevelData {

	struct ObjectData {
		// �t�@�C����
		std::string fileName;
		// ���s�ړ�
		DirectX::XMVECTOR translation;
		// ��]�p
		DirectX::XMVECTOR rotation;
		// �X�P�[�����O
		DirectX::XMVECTOR scaling;
	};

	// �I�u�W�F�N�g�z��
	std::vector<ObjectData> objects;
};

class JsonLoader
{

public:

	static LevelData* LoadFile(const std::string filename);

public:

	//json�t�@�C���i�[���[�g�p�X
	static const std::string kDefaultbaseDirectory;

	static const std::string kExtension;

};

