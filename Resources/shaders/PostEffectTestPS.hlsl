#include "PostEffectTest.hlsli"

texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float Gaussian(float2 drawUV,float2 pickUV,float sigma){
	float d = distance(drawUV,pickUV);
	return exp(-(d * d) / (2 * sigma * sigma)); 
}

bool InSphere(float2 spherePos, float sphereRad, float2 uv){
     float a = uv.x - spherePos.x;
     float b = uv.y - spherePos.y;
	 float c = sqrt((a * a) + (b * b)); 

	 bool inSphere = false;
	 if(c <= sphereRad){inSphere = true;}
	 return inSphere;
}

float4 main(VSOutput input) : SV_TARGET
{

	//点(uv)情報
	float windowWidth = 1280;
	float windowHeight = 720;

	//uv座標をワールド座標に変換
	float2 uv = input.uv;
	uv.x = uv.x / (1 / windowWidth);
	uv.y = uv.y / (1 / windowHeight);

	//円情報
	float2 spherePos = float2(windowWidth / 2,windowHeight / 2);
	float sphereRad = 180;

	//----範囲ブラー----

	float4 texcolor = tex.Sample(smp,input.uv);

	//円範囲外ならブラー
	if(!InSphere(spherePos,sphereRad,uv)){

		//ガウシアンブラー
		float totalWeight = 0,_Sigma = 0.005,_StepWidth = 0.001;

		for(float py = -_Sigma * 2;py <= _Sigma * 2;py += _StepWidth){
			for(float px = -_Sigma * 2;px <= _Sigma * 2;px += _StepWidth){

				float2 pickUV = input.uv + float2(px,py);
				float weight = Gaussian(input.uv,pickUV,_Sigma);
				texcolor += tex.Sample(smp,pickUV) * weight;
				totalWeight += weight;

			}
		}
		texcolor = texcolor / totalWeight;	
	}

	return texcolor;

}