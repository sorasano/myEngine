#include "MathManager.h"

XMMATRIX MatrixInverse(const XMMATRIX& mat)
{
	//元行列
	XMMATRIX matrix = mat;
	//ここに逆行列が入る
	XMMATRIX result;
	result = DirectX::XMMatrixIdentity();
	//一時的なデータを蓄える
	float buf;

	int i, j, k; //カウンタ
	int n = 4;  //配列の次数

	//掃き出し法
	for (i = 0; i < n; i++) {
		buf = 1 / matrix.r[i].m128_f32[i];
		for (j = 0; j < n; j++) {
			matrix.r[i].m128_f32[j] *= buf;
			result.r[i].m128_f32[j] *= buf;
		}
		for (j = 0; j < n; j++) {
			if (i != j) {
				buf = matrix.r[j].m128_f32[i];
				for (k = 0; k < n; k++) {
					matrix.r[j].m128_f32[k] -= matrix.r[i].m128_f32[k] * buf;
					result.r[j].m128_f32[k] -= result.r[i].m128_f32[k] * buf;
				}
			}
		}
	}

	return result;

}

void VecAddXMFLOAT3(XMFLOAT3& f3, const Vector3& v3)
{
	f3.x += v3.x;
	f3.y += v3.y;
	f3.z += v3.z;
}
