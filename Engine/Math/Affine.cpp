/**
* @file Affine.cpp
* @brief アフィン変換
*/


#include "Affine.h"

Affine::Affine() {

	this->Afiine_ = {};
	this->Scale_ = { 1.0f,1.0f,1.0f };
	this->Rot_ = { 0.0f,0.0f,0.0f };
	this->Trans_ = { 0.0f,  0.0f, 0.0f };

}

Affine::~Affine() {

}

//ワールド変換行列

//拡大縮小
Matrix4 Affine::Scale(Vector3 Scale) {

	//スケーリング行列を宣言
	Matrix4 matScale;

	//スケーリング倍率を行列を設定する
	matScale = { Scale.x, 0.0f, 0.0f,    0.0f, 0.0f, Scale.y, 0.0f, 0.0f,
				0.0f,    0.0f, Scale.z, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f };

	return matScale;
};

//回転X
Matrix4 Affine::RotX(float roteX) {

	Matrix4 matRotX;

	// X軸回転行列の各要素を設定する
	matRotX = { 1.0f, 0.0f,        0.0f,       0.0f, 0.0f, cos(roteX), sin(roteX), 0.0f,
			   0.0f, -sin(roteX), cos(roteX), 0.0f, 0.0f, 0.0f,       0.0f,       1.0f };

	return matRotX;
};

//回転Y
Matrix4 Affine::RotY(float roteY) {

	Matrix4 matRotY;

	// Y軸回転行列の各要素を設定する

	matRotY = { cos(roteY), 0.0f, -sin(roteY), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			   sin(roteY), 0.0f, cos(roteY),  0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	return matRotY;
};

//回転Z
Matrix4 Affine::RotZ(float roteZ) {

	Matrix4 matRotZ;

	// Z軸回転行列の各要素を設定する

	matRotZ = { cos(roteZ), sin(roteZ), 0.0f, 0.0f, -sin(roteZ), cos(roteZ), 0.0f, 0.0f,
			   0.0f,       0.0f,       1.0f, 0.0f, 0.0f,        0.0f,       0.0f, 1.0f };

	return matRotZ;
};

//回転合成
Matrix4 Affine::Rot(Matrix4 RotX, Matrix4 RotY, Matrix4 RotZ) {

	// 合成用回転行列を宣言
	Matrix4 matRot;

	//各軸の回転行列を合成
	matRot = (RotZ *= RotX *= RotY);

	return matRot;
};

//平行移動
Matrix4 Affine::Trans(Vector3 Trans) {

	//スケーリング行列を宣言
	Matrix4 matTrans = Identity();

	//スケーリング倍率を行列を設定する
	matTrans = { 1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f,  1.0f,    0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				Trans.x, Trans.y, Trans.z, 1.0f };

	return matTrans;
};

//ワールド変換行列
Matrix4 Affine::World(Matrix4 Scale, Matrix4 Rot, Matrix4 Trans) {

	Matrix4 worldTransform_;

	//行列の合成

	//単位行列を代入する
	worldTransform_ = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.f,  0.0f, 1.0f };

	//掛け算して代入する
	worldTransform_ *= Scale *= Rot *= Trans;

	////行列の転送
	// worldTransform.TransferMatrix();

	return worldTransform_;
};