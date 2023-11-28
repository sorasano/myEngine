#include "MathManager.h"

XMMATRIX MatrixInverse(XMMATRIX mat)
{
	//ここに逆行列が入る
	XMMATRIX result;
	result = DirectX::XMMatrixIdentity();
	//一時的なデータを蓄える
	float buf;

	int i, j, k; //カウンタ
	int n = 4;  //配列の次数

	//掃き出し法
	for (i = 0; i < n; i++) {
		buf = 1 / mat.r[i].m128_f32[i];
		for (j = 0; j < n; j++) {
			mat.r[i].m128_f32[j] *= buf;
			result.r[i].m128_f32[j] *= buf;
		}
		for (j = 0; j < n; j++) {
			if (i != j) {
				buf = mat.r[j].m128_f32[i];
				for (k = 0; k < n; k++) {
					mat.r[j].m128_f32[k] -= mat.r[i].m128_f32[k] * buf;
					result.r[j].m128_f32[k] -= result.r[i].m128_f32[k] * buf;
				}
			}
		}
	}

	return result;

}