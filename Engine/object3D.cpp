#include "object3D.h"

//3D�I�u�W�F�N�g������
void InitializeObject3d(Object3d2* object, ComPtr<ID3D12Device> device_)
{
	HRESULT result;

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform_)
	);
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
	result = object->constBuffTransform_->Map(0, nullptr, (void**)&object->constMapTransform_);
	assert(SUCCEEDED(result));
}

void UpdateObject3d(Object3d2* object, XMMATRIX& matView_, XMMATRIX& matProjection_)
{
	XMMATRIX matScale, matRot, matTrans;

	matScale = XMMatrixScaling(object->scale_.x, object->scale_.y, object->scale_.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation_.z);
	matRot *= XMMatrixRotationX(object->rotation_.x);
	matRot *= XMMatrixRotationY(object->rotation_.y);

	matTrans = XMMatrixTranslation(object->position_.x, object->position_.y, object->position_.z);

	//���[���h�s��̍���
	object->matWorld_ = XMMatrixIdentity();
	object->matWorld_ *= matScale;
	object->matWorld_ *= matRot;
	object->matWorld_ *= matTrans;

	//�e�I�u�W�F�N�g�������
	if (object->parent != nullptr)
	{
		object->matWorld_ *= object->parent->matWorld_;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	object->constMapTransform_->mat = object->matWorld_ * matView_ * matProjection_;
}

//�I�u�W�F�N�g�`�揈��
void DrawObject3d(Object3d2* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView_,
	D3D12_INDEX_BUFFER_VIEW& ibView_, UINT numIndices)
{
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform_->GetGPUVirtualAddress());
	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView_);
	//�`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);	//�S�Ă̒��_���g���ĕ`��
}

void Object3D::Initialize(DirectXCommon* dx, Model* model)
{
	this->dx_ = dx;
	this->model_ = model;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);

	HRESULT result;

	// �萔�o�b�t�@�̐���
	result = this->dx_->GetDevice()->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));

	// ���\�[�X�ݒ�
	resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = this->dx_->GetDevice()->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffB1));
	assert(SUCCEEDED(result));
}

void Object3D::Update(XMMATRIX& matView, XMMATRIX& matProjection)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation_.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation_.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation_.y));
	matTrans = XMMatrixTranslation(position_.x, position_.y, position_.z);

	// ���[���h�s��̍���
	matWorld_ = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld_ *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld_ *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld_ *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	//// �e�I�u�W�F�N�g�������
	//if (parent != nullptr) {
	//	// �e�I�u�W�F�N�g�̃��[���h�s����|����
	//	matWorld *= parent->matWorld;
	//}

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->mat = matWorld_ * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB1* constMap1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	constMap1->ambient = model_->GetMaterial().ambient;
	constMap1->diffuse = model_->GetMaterial().diffuse;
	constMap1->specular = model_->GetMaterial().specular;
	constMap1->alpha = model_->GetMaterial().alpha;
	constBuffB1->Unmap(0, nullptr);
}

void Object3D::Draw(D3D12_VERTEX_BUFFER_VIEW& vbView,D3D12_INDEX_BUFFER_VIEW& ibView)
{
	this->cmdList_ = dx_->GetCommandList();
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList_->SetPipelineState(model_->GetPipelinestate().Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList_->SetGraphicsRootSignature(model_->GetRootSignature().Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���_�o�b�t�@�̐ݒ�
	cmdList_->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList_->IASetIndexBuffer(&ibView);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { model_->GetDescHeap().Get()};
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = model_->GetSrv();
	UINT incrementSize = dx_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvGpuHandle.ptr += incrementSize * model_->textureIndex;
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList_->SetGraphicsRootDescriptorTable(2, srvGpuHandle);
	// �`��R�}���h
	cmdList_->DrawIndexedInstanced(model_->GetIndicesSize(), 1, 0, 0, 0);
}

void Object3D::setPosition(XMFLOAT3 pos)
{
	position_ = pos;
}

void Object3D::setRotation(XMFLOAT3 rot)
{
	rotation_ = rot;
}

void Object3D::setScale(XMFLOAT3 sca)
{
	scale_ = sca;
}

Object3D* Object3D::GetInstance()
{
	static Object3D instance;
	return &instance;
}

Object3D::Object3D()
{
}

Object3D::~Object3D()
{
}
