#pragma once
#include "d3dx12.h"
#include "DirectXMath.h"
#include "array"
#include "SpriteManager.h"
#include "Easing.h"

class Sprite
{
private:	//�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:	//�T�u�N���X
	//�萔�o�b�t�@
	struct ConstBuffMaterial
	{
		XMFLOAT4 color;
	};
	//�萔�o�b�t�@2
	struct ConstBuffTransform
	{
		XMMATRIX mat;	//3D�ϊ��s��
	};
	//���_�f�[�^�p�\����
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

public:	//�����o�֐�
	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList_);

public:	//�ÓI�����o�ϐ��Z�b�^�[
	static void SetDevice(ID3D12Device* device) { Sprite::device_ = device; }
	static void SetSpriteManager(SpriteManager* spriteManager) { Sprite::spriteManager_ = spriteManager; }
	static void CreateGraphicsPipeLine();

public:	//�Z�b�^�[
	//�A���t�@�l
	void SetAlpha(float alpha) { color.w = alpha; }
	//�F
	void SetColor(XMFLOAT3 c) { color.x = c.x; color.y = c.y; color.z = c.z; }
	//�e�N�X�`���̔ԍ����Z�b�g
	void SetTextureNum(int num) { textureNum_ = num; }
	//���W
	void SetPosition(XMFLOAT2 pos) { position_ = pos; }
	//�p�x
	void SetRotation(float rot) { rotation_ = rot; }
	//�X�P�[��
	void SetScale(XMFLOAT2 sca) { scale_ = sca; }

	void SetAnchorPoint(const DirectX::XMFLOAT2& point) { anchorPoint_ = point; }

public:	//�Q�b�^�[
	//���W
	XMFLOAT2 GetPosition() { return position_; }
	//�p�x
	float GetRotation() { return rotation_; }
	//�X�P�[��
	XMFLOAT2 GetScale() { return scale_; }

private:
	//�f�o�C�X
	static ID3D12Device* device_;
	//�X�v���C�g�}�l�[�W���[
	static SpriteManager* spriteManager_;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature>rootsignature_;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState>pipelinestate_;

private:	//�����o�ϐ�
	//�g�p����e�N�X�`���̔ԍ�
	int textureNum_ = 0;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//���_�f�[�^
	Vertex vertices_[6] = {};
	Vertex* vertMap_ = nullptr;
	//�萔�o�b�t�@ �}�e���A��
	ComPtr<ID3D12Resource>constBuffMaterial_;
	ConstBuffMaterial* constMapMaterial_ = nullptr;
	//�萔�o�b�t�@ �ό`�s��
	ComPtr<ID3D12Resource>constBuffTransform_;
	ConstBuffTransform* constMapTransform_ = nullptr;

public:

	//�e�N�X�`���̐F
	XMFLOAT4 color = { 1,1,1,1 };

private:

	float rotation_ = 0;
	XMFLOAT2 position_ = { 0,0 };
	XMFLOAT2 scale_ = { 100.0f,100.0f };

	//�A���J�[�|�C���g(���W�ϊ��̊�_)
	DirectX::XMFLOAT2 anchorPoint_ = { 0.0f,0.0f };
};

