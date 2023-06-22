#pragma once
#include<d3d12.h>
#include<vector>
#include<wrl.h>
#include"DirectXCommon.h"


#define BLEND_NOBLEND	(0)	//�u�����h����
#define BLEND_ALPHA		(1)	//���u�����h
#define BLEND_ADD		(2)	//���Z�u�����h
#define BLEND_SUB		(3)	//���Z�u�����h
#define BLEND_INV		(4)	//���]�u�����h

class GpPipeline
{
public:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> state;//�p�C�v���C���X�e�[�g
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};//�p�C�v���C���f�X�N
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

public:
	void SetPipeline(ID3DBlob* vsBlob, ID3DBlob* psBlob, std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout_, int blendmode = BLEND_NOBLEND,int primitiveNum = 0);
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pipelineState_);
	void SetBlendAlpha();
	void SetBlendAdd();
	void SetBlendSub();
	void SetBlendInv();

};

