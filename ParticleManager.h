#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include "Base/DirectXCommon.h"

using namespace Microsoft::WRL;

class ParticleManager
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{

		XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX matbBillboard;
	};

	struct Particle {
		//DirectX::���ȗ�
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//���W
		XMFLOAT3 position = {};
		//���x
		XMFLOAT3 velocity = {};
		//�����x
		XMFLOAT3 accel = {};
		//���݃t���[��
		int frame = 0;
		//�I���t���[��
		int num_flame = 0;

	};

private: // �萔
	const int division = 50;					// ������
	const float radius = 5.0f;				// ��ʂ̔��a
	const float prizmHeight = 8.0f;			// ���̍���
	const int planeCount = division * 2 + division * 2;		// �ʂ̐�

	static const int vertexCount = 1024;		// ���_��

public: // �ÓI�����o�֐�


	ParticleManager();

	~ParticleManager();

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	static void StaticInitialize(DirectXCommon* dx, int window_width, int window_height);

	/// <summary>
	/// �ÓI�X�V
	/// </summary>
	/// <param name="eye">���_���W</param>
	/// <param name="target">���_�������W</param>
	static void StaticUpdate(XMFLOAT3 eye, XMFLOAT3 target);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	ParticleManager* Create(DirectXCommon* dx_, const std::string& resourcename);

	/// <summary>
	/// ���_���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const XMFLOAT3& GetEye() { return eye; }

	/// <summary>
	/// ���_���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetEye(XMFLOAT3 eye);

	/// <summary>
	/// �����_���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const XMFLOAT3& GetTarget() { return target; }

	/// <summary>
	/// �����_���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetTarget(XMFLOAT3 target);

	/// <summary>
	/// �x�N�g���ɂ��ړ�
	/// </summary>
	/// <param name="move">�ړ���</param>
	void CameraMoveVector(XMFLOAT3 move);

	void CameraMoveEyeVector(XMFLOAT3 move);

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �R�}���h���X�g
	ID3D12GraphicsCommandList* cmdList;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//// �C���f�b�N�X�o�b�t�@
	// ComPtr<ID3D12Resource> indexBuff;
	//// �e�N�X�`���o�b�t�@
	// ComPtr<ID3D12Resource> texbuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// ���_���W
	static XMFLOAT3 eye;
	// �����_���W
	static XMFLOAT3 target;
	// ������x�N�g��
	static XMFLOAT3 up;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;
	// ���_�f�[�^�z��
	static VertexPos vertices[vertexCount];
	//// ���_�C���f�b�N�X�z��
	// unsigned short indices[indexCount];

	//�r���[�{�[�h�s��
	static XMMATRIX matBillbord;
	//Y������̃r���[�{�[�h�s��
	static XMMATRIX matBillbordY;

	static DirectXCommon* dx;

private:// �ÓI�����o�֐�
	/// <summary>
	/// �f�X�N���v�^�q�[�v�̏�����
	/// </summary>
	void InitializeDescriptorHeap();

	/// <summary>
	/// �J����������
	/// </summary>
	/// <param name="window_width">��ʉ���</param>
	/// <param name="window_height">��ʏc��</param>
	static void InitializeCamera(int window_width, int window_height);

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	/// <returns>����</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTexture();

	void LoadTexture(const std::string& resourcename);

	/// <summary>
	/// ���f���쐬
	/// </summary>
	void CreateModel();

	/// <summary>
	/// �r���[�s����X�V
	/// </summary>
	static void UpdateViewMatrix();

public: // �����o�֐�
	bool Initialize(const std::string& resourcename);
	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	//�p�[�e�B�N���̒ǉ�
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel);

private: // �����o�ϐ�

	ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;

	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };

	//�p�[�e�B�N���z��
	std::forward_list<Particle> particles;
};

