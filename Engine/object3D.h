#pragma once
#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "assert.h"
#include <DirectXTex.h>

#include "wrl.h"
#include "Model.h"
#include "Base/DirectXCommon.h"

using namespace DirectX;
using namespace Microsoft::WRL;

////���_�f�[�^�\����
//struct Vertex
//{
//	XMFLOAT3 pos;	//���W
//	XMFLOAT3 normalize;	//�@���x�N�g��
//	XMFLOAT2 uv;	//uv���W
//	Vertex* parent = nullptr;
//};
//
//struct Indices
//{
//	int num;
//};

//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//�F(RGBA)
};

//�萔�o�b�t�@�p�f�[�^�\���́i3D�ϊ��s��j
struct ConstBufferDataTransform
{
	XMMATRIX mat;
};

//3D�I�u�W�F�N�g�^
struct Object3d2
{
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform_;
	//�萔�o�b�t�@�}�b�v
	ConstBufferDataTransform* constMapTransform_;
	//�A�t�B���ϊ����
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld_;
	//�e�I�u�W�F�N�g�̃|�C���^�[
	Object3d2* parent = nullptr;
};

//3�I�u�W�F�N�g�֘A
//3D�I�u�W�F�N�g������
void InitializeObject3d(Object3d2* object, ComPtr<ID3D12Device> device);
//3D�I�u�W�F�N�g�X�V
void UpdateObject3d(Object3d2* object, XMMATRIX& matView, XMMATRIX& matProjection);
//�I�u�W�F�N�g�`�揈��
void DrawObject3d(Object3d2* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

class Object3D
{
public:
	//�V���O���g���C���X�^���X
	Object3D* GetInstance();
	Object3D();
	~Object3D();
	void Initialize(DirectXCommon* dx,Model* model);
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	void Draw(D3D12_VERTEX_BUFFER_VIEW& vbView,D3D12_INDEX_BUFFER_VIEW& ibView);
	//�Q�b�^�[�@�Z�b�^�[�@
	XMFLOAT3 GetPosition() { return position_; };
	XMFLOAT3 GetRotation() { return rotation_; };
	XMFLOAT3 GetScale() { return scale_; };
	void setPosition(XMFLOAT3 pos);
	void setRotation(XMFLOAT3 rot);
	void setScale(XMFLOAT3 sca);
public:
	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		XMMATRIX mat;	// �R�c�ϊ��s��
	};
	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;	//�A���r�G���g�W��
		float pad1;			//�p�f�B���O
		XMFLOAT3 diffuse;	//�f�B�t���[�Y�W��
		float pad2;			//�p�f�B���O
		XMFLOAT3 specular;	//�X�y�L�����[�W��
		float alpha;		//�A���t�@
	};
private:
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform_;
	//�萔�o�b�t�@�}�b�v
	ConstBufferDataTransform* constMapTransform_;
	//���f��
	Model* model_ = nullptr;
	//�f�o�C�X
	DirectXCommon* dx_ = nullptr;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB0; 
	ComPtr<ID3D12Resource> constBuffB1;
	// �R�}���h���X�g
	ID3D12GraphicsCommandList* cmdList_;
private:
	//�A�t�B���ϊ����
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0};
	XMFLOAT3 position_ = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld_;
};