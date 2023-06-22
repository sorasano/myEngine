#include "Sprite.h"
using namespace DirectX;
#include<wrl.h>
using namespace Microsoft::WRL;
using namespace std;
#include"WinApp.h"

void Sprite::Initialize(uint32_t textureNum)
{
	HRESULT result{  };

	spriteManager = SpriteManager::GetInstance();
	ComPtr<ID3D12Device> dev = spriteManager->directX->GetDevice();

	if (textureNum != UINT32_MAX) {
		textureIndex = textureNum;
		AdjustTextureSize();
		//�e�N�X�`���T�C�Y���X�v���C�g�̃T�C�Y�ɓK�p
		size = textureSize;
	}

	//���_�f�[�^
	VertexPosUv vertices_[] = {
		//x    y     z   
		{{  0.0f,100.0f,0.0f},{0.0f,1.0f}},//����
		{{  0.0f,  0.0f,0.0f},{0.0f,0.0f}},//����
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},//�E��
		{{100.0f,  0.0f,0.0f},{1.0f,0.0f}},//�E��
	};

	for (int i = 0; i < _countof(vertices_); i++) {
		vertices[i] = vertices_[i];
	}

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosUv) * _countof(vertices_));

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@����
	result = dev.Get()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	//�Ȃ��������
	vertBuff->Unmap(0, nullptr);


	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_�P���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(VertexPosUv);

	//�萔�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@����
	result = dev->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	//�l���������ނƎ����I�ɓ]�������
	constMap->color = XMFLOAT4(1, 1, 1, 1);

	//�s��ɂ͂Ƃ肠�����P�ʍs�����
	constMap->mat = XMMatrixIdentity();

	constMap->mat.r[0].m128_f32[0] = 2.0f / WinApp::winW;
	constMap->mat.r[1].m128_f32[1] = -2.0f / WinApp::winH;
	constMap->mat.r[3].m128_f32[0] = -1.0f;
	constMap->mat.r[3].m128_f32[1] = 1.0f;
	matWorld = XMMatrixIdentity();
	constMap->mat = matWorld;
}

void Sprite::Draw()
{
	//��\���Ȃ珈���I��
	if (isInvisible) {
		return;
	}

	//�e�N�X�`���̐ݒ�R�}���h
	spriteManager->SetTextureCommand(textureIndex);

	Update();

	//���_�o�b�t�@�r���[�̐ݒ�
	spriteManager->directX->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
	//�萔�o�b�t�@�r���[
	spriteManager->directX->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//�`��R�}���h
	spriteManager->directX->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);
}

void Sprite::Update()
{
	HRESULT result;

	//���_�f�[�^�������o�ϐ��Ōv�Z
	float left = (0.0f - anchorPoint.x) * size.x;
	float right = (1.0f - anchorPoint.x) * size.x;
	float top = (0.0f - anchorPoint.y) * size.y;
	float bottom = (1.0f - anchorPoint.y) * size.y;

	//���E���]
	if (isFlipX) {
		left = -left;
		right = -right;
	}

	//�㉺���]
	if (isFlipY) {
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,bottom,0.0f };
	vertices[LT].pos = { left,top,0.0f };
	vertices[RB].pos = { right,bottom,0.0f };
	vertices[RT].pos = { right,top,0.0f };

	//�e�N�X�`���o�b�t�@�擾
	ID3D12Resource* textureBuffer = spriteManager->GetTextureBuffer(textureIndex);
	//�w��ԍ��̃e�N�X�`�����ǂݍ��ݍς݂Ȃ�
	if (textureBuffer) {
		//�e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop.x / resDesc.Width;
		float tex_right = (textureLeftTop.x + textureSize.x) / resDesc.Width;
		float tex_top = textureLeftTop.y / resDesc.Height;
		float tex_bottom = (textureLeftTop.y + textureSize.y) / resDesc.Height;
		//���_��UV�ɔ��f
		vertices[LB].uv = { tex_left,tex_bottom };
		vertices[LT].uv = { tex_left,tex_top };
		vertices[RB].uv = { tex_right,tex_bottom };
		vertices[RT].uv = { tex_right,tex_top };

	}

	//���_�o�b�t�@�Ƀf�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	//�Ȃ��������
	vertBuff->Unmap(0, nullptr);

	matWorld = XMMatrixIdentity();
	XMMATRIX matRot = XMMatrixIdentity();
	XMMATRIX matTrans = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation);
	matTrans = XMMatrixTranslation(position.x, position.y, 0.0f);
	matWorld *= matRot;
	matWorld *= matTrans;

	XMMATRIX matProjection = XMMatrixIdentity();

	matProjection.r[0].m128_f32[0] = 2.0f / WinApp::winW;
	matProjection.r[1].m128_f32[1] = -2.0f / WinApp::winH;
	matProjection.r[3].m128_f32[0] = -1.0f;
	matProjection.r[3].m128_f32[1] = 1.0f;

	matWorld *= matProjection;

	constMap->mat = matWorld;

	//�F��]��
	constMap->color = color;

	if (isflipEase && endFlip == false) {
		FlipOut();
	}
	else if (isSway) {
		Sway();
	}

}

void Sprite::FlipOut()
{

	float flipInUp = startEasePos.y - flipInRangeUp;
	float flipInDown = startEasePos.y + flipInRangeDown;

	if (initFlip) {

		if (flipInFase == 0) {

			position = EaseOut(startEasePos, { startEasePos.x,flipInUp }, flipInEase.timeRate);

			if (!flipInEase.GetActive()) {
				//���̉��o���I������玟�̒i�K��
				flipInFase = 1;
				flipInDown = position.y + flipInRangeDown;

				//���l�̃��Z�b�g
				startEasePos = position;
				flipInEase.Start(1.0f);
				flipInEase.timeRate = 0.0f;
			}

		}
		else if (flipInFase == 1) {

			position = EaseOut(startEasePos, { startEasePos.x,flipInDown }, flipInEase.timeRate);

			if (!flipInEase.GetActive()) {
				//���̉��o���I������玟�̒i�K��
				flipInFase = 2;

				//���l�̃��Z�b�g
				startEasePos = position;
				flipInEase.Start(1.0f);
				flipInEase.timeRate = 0.0f;
			}


		}
		else if (flipInFase == 2) {

			position = EaseOut(startEasePos, { startEasePos.x, -100 }, flipInEase.timeRate);

			if (!flipInEase.GetActive()) {
				//���̉��o���I������玟�̒i�K��
				flipInFase = 3;

				//���l�̃��Z�b�g
				//isflipEase = false;
				//initFlip = false;
				endFlip = true;
			}

		}

	}
	else {
		initFlip = true;
		flipInFase = 0;
		flipInUp = position.y - flipInRangeUp;
		flipInEase.Start(1.0f);
		flipInEase.timeRate = 0.0f;
		startEasePos = position;
		isSway = false;
	}

	flipInEase.Update();
}

void Sprite::Sway()
{

	float swayUp = swayCenterPos.y - (swayRange / 2);
	float swayDown = swayCenterPos.y + (swayRange / 2);

	if (initSway) {

		if (isSwayUp) {
			position = EaseOut(startEasePos, { startEasePos.x,swayUp }, swayEase.timeRate);

			if (!swayEase.GetActive()) {
				//��܂ōs�����牺�~�ɕύX
				isSwayUp = false;

				//���l�̃��Z�b�g
				startEasePos = position;
				swayEase.Start(2.0f);
				swayEase.timeRate = 0.0f;
			}
		}
		else {
			position = EaseOut(startEasePos, { startEasePos.x,swayDown }, swayEase.timeRate);

			if (!swayEase.GetActive()) {
				//���܂ōs������㏸�ɕύX
				isSwayUp = true;
				//���l�̃��Z�b�g
				startEasePos = position;
				swayEase.Start(2.0f);
				swayEase.timeRate = 0.0f;
			}
		}

	}
	else {
		initSway = true;
		swayEase.Start(1.0f);
		startEasePos = position;
	}

	swayEase.Update();
}

void Sprite::AdjustTextureSize() {
	ID3D12Resource* textureBuffer = spriteManager->GetTextureBuffer(textureIndex);
	assert(textureBuffer);

	//�e�N�X�`�����擾
	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize.x = static_cast<float>(resDesc.Width);
	textureSize.y = static_cast<float>(resDesc.Height);
}


