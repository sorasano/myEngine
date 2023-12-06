/**
* @file Camera.cpp
* @brief カメラの処理
*/

#include "Camera.h"
#include"Base/WinApp.h"
#include "Imgui.h"

using namespace DirectX;

ID3D12Device* Camera::device_ = nullptr;

void Camera::StaticInitialize(ID3D12Device* dev)
{
	//NULLチェック
	assert(dev);
	device_ = dev;
}

void Camera::Initialize()
{
	//引数をメンバに移して行列更新
	this->input_ = Input::GetInstance();

	HRESULT result;
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferCamera) + 0xff) & ~0xff;	//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = constBuff_->Map(0, nullptr, (void**)&constMap);//マッピング
	assert(SUCCEEDED(result));

	UpdateMatrix();
}

void Camera::UpdateMatrix()
{

	//ビュー変換行列の計算
	matView_ = XMMatrixLookAtLH(XMLoadFloat3(&eye_), XMLoadFloat3(&target_), XMLoadFloat3(&up_));

	//描画距離の更新
	float farClip = eye_.z + farClip_;

	//専用の行列を宣言
	matProjection_ = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),					//上下画角45度
		(float)window_width / window_height,	//アスペクト比（画面横幅/画面縦幅）
		nearClip_, farClip								//前橋、奥橋
	);

	HRESULT result;

	//定数バッファへデータ転送
	ConstBufferCamera* constMapCamera = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMapCamera);
	if (SUCCEEDED(result))
	{
		constMap->viewproj = matView_ * matProjection_;
		constMap->cameraPos = eye_;
		constBuff_->Unmap(0, nullptr);
	}
}

void Camera::Update(const XMFLOAT3& playerPos)
{
	//データ更新
	this->playerPos_ = playerPos;

	switch (mode_) {
	case STRAIGHTMODE:
		UpdateStraightMode();
		break;

	case PLAYERFOLLOWMODE:
		UpdatePlayerFollowMode();
		break;
	case PLAYERTURNINGMODE:
		UpdatePlayerTurningMode();
		break;
	case PERFORMANCEMODE:
		break;
	}

	//DebugMode();

	UpdateMatrix();

}

void Camera::UpdateStraightMode()
{

	eye_.x = initEye_.x;
	eye_.y = initEye_.y;
	target_.x = initTarget_.x;
	target_.y = initTarget_.y;

	//一定スピードで進み続ける
	eye_.z += straightModeSpeed_;
	target_.z = eye_.z + playerRange_;
}

void Camera::UpdatePlayerFollowMode()
{
	eye_.x = initEye_.x;
	eye_.y = initEye_.y;
	target_.x = initTarget_.x;
	target_.y = initTarget_.y;

	//プレイヤーの後ろからプレイヤーを追従する視点
	eye_.z = playerPos_.z - playerRange_;
	target_.z = playerPos_.z;
}

void Camera::UpdatePlayerTurningMode()
{

	//視点は常にプレイやー
	target_ = playerPos_;

	//高さ(Y)は固定
	eye_.y = playerPos_.y;

	//XZは旋回
	{
		// 中心座標に角度と長さを使用した円の位置を加算する
		// 度数法の角度を弧度法に変換
		float radius = angle_ * 3.14f / 180.0f;

		// 三角関数を使用し、円の位置を割り出す。
		float addX = (float)(cos(radius) * turnLength_);
		float addZ = (float)(sin(radius) * turnLength_);

		// 結果ででた位置を中心位置に加算し、それを描画位置とする
		eye_.x = playerPos_.x + addX;
		eye_.z = playerPos_.z + addZ;

		// 角度更新
		angle_ += addAngle_;
	}
}

void Camera::DebugMode()
{
	//カメラ操作
	float speed = 1.0f;

	if (input_->IsKeyPress(DIK_W) || input_->IsKeyPress(DIK_S) || input_->IsKeyPress(DIK_D) || input_->IsKeyPress(DIK_A)) {

		//座標を移動する処理
		if (input_->IsKeyPress(DIK_W)) {
			eye_.z += speed;
		}
		else if (input_->IsKeyPress(DIK_S)) {
			eye_.z -= speed;
		}

		if (input_->IsKeyPress(DIK_A)) {
			eye_.x -= speed;
		}
		else if (input_->IsKeyPress(DIK_D)) {
			eye_.x += speed;
		}

	}

	if (input_->IsKeyPress(DIK_UP) || input_->IsKeyPress(DIK_DOWN) || input_->IsKeyPress(DIK_RIGHT) || input_->IsKeyPress(DIK_LEFT)) {

		//座標を移動する処理
		if (input_->IsKeyPress(DIK_UP)) {
			target_.y += speed;
		}
		else if (input_->IsKeyPress(DIK_DOWN)) {
			target_.y -= speed;
		}

		if (input_->IsKeyPress(DIK_RIGHT)) {
			target_.x -= speed;
		}
		else if (input_->IsKeyPress(DIK_LEFT)) {
			target_.x += speed;
		}

	}
}

void Camera::Reset()
{
	eye_ = initEye_;
	target_ = initTarget_;

	mode_ = STRAIGHTMODE;
}
