cbuffer cbuff0 : register(b0)
{

	matrix mat; // �R�c�ϊ��s��
	matrix matBillboard;
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 pos : SV_POSITION; // �V�X�e���p���_���W

};

struct GSOutput {
	float4 svpos : SV_POSITION;//���_���W

	float2 uv :TEXCOORD;//uv
};
