#include "Obj.hlsli"

VSOutput main(float4 pos:POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//VSOutput output;	//�s�N�Z���V�F�[�_�ɓn���l
	//output.svpos = mul(mat, pos);
	//output.normal = normal;
	//output.uv = uv;

	//�E,��,���_�k�����������C�g
	float3 lightdir = float3(1,-1,1);
	lightdir = normalize(lightdir);
	//���C�g�̐F
	float3 lightcolor = float3(1,1,1);
	//�s�N�Z���V�F�[�_�ɓn���l
	VSOutput output;
	output.svpos = mul(mat,pos);
	//lambert���˂̌v�Z
	output.color.rgb = dot(-lightdir,normal) , m_diffuse , lightcolor;
	output.color.a = m_alpha;
	output.uv = uv;

	output.normal = normal;
	
	return output;
}