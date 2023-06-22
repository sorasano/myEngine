#pragma once


#include<DirectXMath.h>
#include"Texture.h"
#include<string>
#include"Vector3.h"
#include"SpriteManager.h"
#include"Easing.h"

enum VertexNumber {
	LB,	//����
	LT,	//����
	RB,	//�E��
	RT,	//�E��
};

class Sprite
{

private:


public:	//�����o�ϐ�

	//�}�l�[�W���N���X�̃|�C���^
	SpriteManager* spriteManager = nullptr;

	VertexPosUv vertices[4] = {};		//���_���W�f�[�^
	D3D12_VERTEX_BUFFER_VIEW vbView{};	//���_�o�b�t�@�r���[
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;	//���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff = nullptr;	//�萔�o�b�t�@
	uint32_t textureIndex = 0;	//�e�N�X�`���Ɋ��蓖�Ă��Ă���ԍ�
	ConstBufferData* constMap = nullptr;//�萔�o�b�t�@�\����

	DirectX::XMMATRIX matWorld{};	//���[���h�ϊ��s��
	float rotation = 0.0f;	//��]�p
	DirectX::XMFLOAT2 position = { 0.0f,0.0f };	//���W
	DirectX::XMFLOAT4 color = { 1,1,1,1 };	//���W
	DirectX::XMFLOAT2 size = { 100.0f,100.0f };//�X�v���C�g�T�C�Y
	DirectX::XMFLOAT2 anchorPoint = { 0.0f,0.0f };//�A���J�[�|�C���g(���W�ϊ��̊�_)
	bool isFlipX = false;	//���E���]�t���O
	bool isFlipY = false;	//�㉺���]�t���O
	bool isInvisible = false;	//��\���t���O

	DirectX::XMFLOAT2 textureLeftTop = { 0.0f,0.0f };	//�e�N�X�`��������W
	DirectX::XMFLOAT2 textureSize = { 100.0f,100.0f };	//�e�N�X�`���؂�o���T�C�Y

	//��ʊO��������Ă��鉉�o�p
	Easing flipInEase;
	//�t���b�v������
	bool initFlip = false;
	//���o����
	bool isflipEase = false;
	//���o�̒i�K
	int flipInFase;
	//�h�ꕝ
	float flipInRangeUp = 50;
	float flipInRangeDown = 100;
	//���o���I�������
	bool endFlip = false;

	//�h��p
	Easing swayEase;
	//�h�ꏉ����
	bool initSway = false;
	//�h�ꕝ
	float swayRange = 100;
	//�h��Ă邩
	bool isSway = false;
	//�㏸�������~����
	bool isSwayUp = true;
	//���o�J�n�ʒu
	DirectX::XMFLOAT2 startEasePos;
	//���S�_
	DirectX::XMFLOAT2 swayCenterPos;


public: //�����o�֐�
	void Initialize(uint32_t textureNum = UINT32_MAX);

	void Draw();
	void SetColor(const DirectX::XMFLOAT4& color_) { color = color_; }
	void SetPos(const DirectX::XMFLOAT2& pos) { position = pos; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetSize(const DirectX::XMFLOAT2& size_) { size = size_; }
	void SetAnchorPoint(const DirectX::XMFLOAT2& point) { anchorPoint = point; }
	void SetFlipX(bool flipX) { isFlipX = flipX; }
	void SetFlipY(bool flipY) { isFlipY = flipY; }
	void SetInvisible(bool flag) { isInvisible = flag; }
	void SetTextureNum(uint32_t index) { textureIndex = index; }
	void SetTextureLeftTop(const DirectX::XMFLOAT2& leftTop) { textureLeftTop = leftTop; }
	void SetTextureSize(const DirectX::XMFLOAT2& size) { textureSize = size; }

	const DirectX::XMFLOAT2& GetPosition()const { return position; }
	float GetRotation()const { return rotation; }
	const DirectX::XMFLOAT4 GetColor()const { return color; }
	const DirectX::XMFLOAT2 GetSize()const { return size; }
	const DirectX::XMFLOAT2 GetAnchorPoint()const { return anchorPoint; }
	bool GetIsFlipX()const { return isFlipX; }
	bool GetIsFlipY()const { return isFlipY; }
	bool GetIsInvisible()const { return isInvisible; }
	uint32_t GetTextureNum()const { return textureIndex; }
	const DirectX::XMFLOAT2 GetTextureLeftTop()const { return textureLeftTop; }
	const DirectX::XMFLOAT2 GetTextureSize()const { return textureSize; }

	void Update();

	//���o

	//��ʊO�փA�E�g
	void StartFlipOut() { isflipEase = true; };
	void FlipOut();

	//�h���	center=���S�_
	void StartSway(DirectX::XMFLOAT2 center) { isSway = true, swayCenterPos = center; }
	void Sway();

private:
	//�e�N�X�`���T�C�Y���C���[�W�T�C�Y�ɍ��킹��
	void AdjustTextureSize();
};

