#include "Obj.hlsli"

Texture2D<float4>tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//float3 light = normalize(float3(1,-1,1));	//右下奥　向きのライト
	//float light_diffuse = saturate(dot(-light,input.normal));
	//float3 shader_color;
	//shader_color = m_ambient;	//アンビエント項
	//shader_color += m_diffuse * light_diffuse;	//ディフューズ項
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * shader_color, texcolor.a * m_alpha);

	//テクスチャマッピング
	float4 texcolor = tex.Sample(smp,input.uv);

	return/* input.color **/ texcolor;

}