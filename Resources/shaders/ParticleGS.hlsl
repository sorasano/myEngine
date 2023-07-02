#include "Particle.hlsli"

//[maxvertexcount(3)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION, 
//	inout TriangleStream< GSOutput > output
//)
//{
//	for (uint i = 0; i < 3; i++)
//	{
//		GSOutput element;
//		element.svpos = input[i].svpos;
//		element.normal = input[i].normal;
//		element.uv = input[i].uv;
//		output.Append(element);
//	}
//}

////3角形から点一つ
//[maxvertexcount(1)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION,
//	inout PointStream< GSOutput > output
//)
//{
//
//	GSOutput element;
//	element.svpos = input[0].svpos;
//	element.normal = input[0].normal;
//	element.uv = input[0].uv;
//	output.Append(element);
//
//}


////3角形から点三つ
//[maxvertexcount(3)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION,
//	inout PointStream< GSOutput > output
//)
//{
//	for (uint i = 0; i < 3; i++)
//	{
//		GSOutput element;
//		element.svpos = input[i].svpos;
//		element.normal = input[i].normal;
//		element.uv = input[i].uv;
//		output.Append(element);
//	}
//}

////3角形から線分一つ
//[maxvertexcount(2)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION,
//	inout LineStream< GSOutput > output
//)
//{
//	GSOutput element;
//
//	//始点
//	element.svpos = input[0].svpos;
//	element.normal = input[0].normal;
//	element.uv = input[0].uv;
//	output.Append(element);
//
//	//終点
//	element.svpos = input[1].svpos;
//	element.normal = input[1].normal;
//	element.uv = input[1].uv;
//	output.Append(element);
//}

////3角形から線分三つ
//[maxvertexcount(6)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION,
//	inout LineStream< GSOutput > output
//)
//{
//	GSOutput element;
//
//	for (uint i = 0; i < 3; i++) {
//		//始点
//		element.svpos = input[i].svpos;
//		element.normal = input[i].normal;
//		element.uv = input[i].uv;
//		output.Append(element);
//
//		if (i == 2) {
//			//終点
//			element.svpos = input[0].svpos;
//			element.normal = input[0].normal;
//			element.uv = input[0].uv;
//		}
//		else {
//			//終点
//			element.svpos = input[i + 1].svpos;
//			element.normal = input[i + 1].normal;
//			element.uv = input[i + 1].uv;
//		}
//		output.Append(element);
//		//output.RestartStrip();
//	}
//}

//3角形の入力から3角形を出力するが途中でUVを加工してタイリング
//[maxvertexcount(3)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION,
//	inout TriangleStream< GSOutput > output
//)
//{
//
//	for (uint i = 0; i < 3; i++) {
//
//		GSOutput element;
//		element.svpos = input[i].svpos;
//		element.normal = input[i].normal;
//
//		//UV2倍
//		element.uv = input[i].uv * 2.0f;
//		output.Append(element);
//	}
//}

////3角形の入力から3角形を2つ出力
//[maxvertexcount(6)]
//void main(
//	triangle VSOutput input[3] : SV_POSITION,
//	inout TriangleStream< GSOutput > output
//)
//{
//	//1つめ
//	for (uint i = 0; i < 3; i++) {
//
//		GSOutput element;
//		element.svpos = input[i].svpos;
//		element.normal = input[i].normal;
//		element.uv = input[i].uv;
//		output.Append(element);
//	}
//	output.RestartStrip();
//
//	//1つめ
//	for (uint i = 0; i < 3; i++) {
//
//		GSOutput element;
//		//x方向に20
//		element.svpos = input[i].svpos + float4(20.0f,0.0f,0.0f,0.0f);
//		element.normal = input[i].normal;
//		//UVを5倍に
//		element.uv = input[i].uv * 5.0f;
//		output.Append(element);
//	}
//
//}

////点から点を出力
//[maxvertexcount(3)]
//void main(
//	point VSOutput input[1] : SV_POSITION,
//	inout TriangleStream< GSOutput > output
//)
//{
//	GSOutput element;
//
//	//頂点情報
//	element.normal = input[0].normal;
//	element.uv = input[0].uv;
//
//	element.svpos = input[0].svpos;
//	output.Append(element);
//
//	element.svpos = input[0].svpos + float4(10.0f,10.0f,0,0);
//	output.Append(element);
//
//	element.svpos = input[0].svpos + float4(10.0f, 0, 0, 0);
//	output.Append(element);
//}

static const uint vnum = 4;

static const float4 offset_array[vnum] = {
	float4(-0.5f,-0.5f,0,0),
	float4(-0.5f,+0.5f,0,0),
	float4(+0.5f,-0.5f,0,0),
	float4(+0.5f,+0.5f,0,0)
};


static const float2 uv_array[vnum] = {
	float2(0,1),
	float2(0, 0),
	float2(1,1),
	float2(1, 0)
};

//点から四角形を出力
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;

	//頂点情報
	for (uint i = 0; i < vnum; i++) {

		float4 offset = mul(matBillboard, offset_array[i]);
		element.svpos = input[0].pos + offset;

		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
		output.Append(element);
	}
}
