#pragma once
#include "array"
#include "DirectXMath.h"
#include "vector"
#include "string"

class CSVLoader
{
private:	//�G�C���A�X
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	using string = std::string;	//std::���ȗ�
public:
	//csv�t�@�C����ǂݍ���ŕϐ��ɑ������
	void LoadCSV(const std::string fileName);

	void Draw();

	//�Q�b�^�[
	XMFLOAT3 GetPosition(int num) { return position_[num]; }
	XMFLOAT3 GetRotation(int num) { return rotation_[num]; }
	XMFLOAT3 GetScale(int num) { return scale_[num]; }

	int GetType(int num) { return type_[num]; }
	bool GetStopInScreen(int num) { return stopInScreen_[num]; }

	//�ǂݍ��񂾃I�u�W�F�N�g�̐�
	int GetSize() { return static_cast<int>(position_.size()); }

private:

	std::vector<XMFLOAT3> position_;
	std::vector<XMFLOAT3> rotation_;
	std::vector<XMFLOAT3> scale_;

	std::vector<int> type_;
	std::vector<bool> stopInScreen_;

	//�t�@�C���i�[���[�g�p�X
	static const std::string kDefaultbaseDirectory;

	static const std::string kExtension;
};