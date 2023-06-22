#include "GpPipeline.h"
#include<cassert>


void GpPipeline::SetPipeline(ID3DBlob* vsBlob, ID3DBlob* psBlob, std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout_, int blendmode, int primitiveNum)
{




	inputLayout = inputLayout_;

	//�f�v�X�X�e���V���X�e�[�g�̍쐬
	desc.DepthStencilState.DepthEnable = true;	//�[�x�e�X�g���s��
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//�������݋���
	desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	//��������΍��i
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g

	// �V�F�[�_�[�̐ݒ�
	desc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	desc.VS.BytecodeLength = vsBlob->GetBufferSize();
	desc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	desc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �w�ʂ��J�����O
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	desc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	//blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA�S�Ẵ`�����l����`��

	//���ʐݒ�i�A���t�@�l�j
	blenddesc.BlendEnable = true;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��  0%�g��


	//
	//�����Ńu�����h�ݒ��ύX
	switch (blendmode) {
	case BLEND_NOBLEND: {
		//�u�����h�ݒ�̃t���O������
		blenddesc.BlendEnable = false;
		break;
	}
	case BLEND_ALPHA: {
		SetBlendAlpha();
		break;
	}
	case BLEND_ADD: {
		SetBlendAdd();
		break;
	}
	case BLEND_SUB: {
		SetBlendSub();
		break;
	}
	case BLEND_INV: {
		SetBlendInv();
		break;
	}

	default: {
		break;
	}
	}



	// ���_���C�A�E�g�̐ݒ�
	desc.InputLayout.pInputElementDescs = inputLayout.data();
	desc.InputLayout.NumElements = inputLayout.size();

	// �}�`�̌`��ݒ�
	if (primitiveNum == 0) {
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
	else {
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}

	// ���̑��̐ݒ�
	desc.NumRenderTargets = 1; // �`��Ώۂ�1��
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	desc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O


}

void GpPipeline::SetPipelineState(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pipelineState_)
{
	HRESULT result;

	//�p�C�v���C���X�e�[�g�ɐݒ�𔽉f
	result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//pipelineState_ = state;
}

void GpPipeline::SetBlendAlpha()
{
	//�����������i�A���t�@�u�����h�j
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-�\�[�X�̃A���t�@�l
}

void GpPipeline::SetBlendAdd()
{
	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;	//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE;	//�f�X�g�̒l��100%�g��
}

void GpPipeline::SetBlendSub()
{
	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	//�f�X�g����\�[�X�����Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;				//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE;				//�f�X�g�̒l��100%�g��

}

void GpPipeline::SetBlendInv()
{
	//�F���]
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f-�f�X�g�J���[�̐F
	blenddesc.DestBlend = D3D12_BLEND_ZERO;				//�g��Ȃ�
}
