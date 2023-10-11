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

void Camera::Initialize(Input* input)
{
	//引数をメンバに移して行列更新
	this->input_ = input;

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
		(float)WinApp::winW / WinApp::winH,	//アスペクト比（画面横幅/画面縦幅）
		nearClip_, farClip								//前橋、奥橋
	);

	constMap->view = matView_;
	constMap->projection = matProjection_;
}

void Camera::Update(XMFLOAT3 playerPos, XMFLOAT3 bossPos)
{
	//データ更新
	this->playerPos_ = playerPos;
	this->bossPos_ = bossPos;


	switch (mode_) {
	case STRAIGHTMODE:
		UpdateStraightMode();
		break;

	case PLAYERFOLLOWMODE:
		UpdatePlayerFollowMode();
		//DebugMode();
		break;

	case TITLETOPLAYMODE:
		UpdateTitleToPlayMode();
		break;
	}


	UpdateMatrix();

	ImGui::Begin("cameraPos");
	ImGui::Text("%f",eye_.z);
	ImGui::End();
}

void Camera::UpdateStraightMode()
{
	//一定スピードで進み続ける
	eye_.z += straightModeSpeed_;
	target_.z = eye_.z + playerRange_;
}

void Camera::UpdatePlayerFollowMode()
{
	//プレイヤーの後ろからプレイヤーを追従する視点
	eye_.z = playerPos_.z - playerRange_;
	target_.z = playerPos_.z;
}

void Camera::UpdateTitleToPlayMode()
{
	if (phase_ == 0) {

		//取得したイージング用の開始位置と終了位置でイージングを行う
		eye_ = EaseIn3D(startEye_, endEye_, easeing_.timeRate);
		target_ = EaseIn3D(startTarget_, endTarget_, easeing_.timeRate);

		if (!easeing_.GetActive()) {
			//演出が終わったら次のフェーズへ
			phase_++;

			//イージング用のデータを設定しなおす
			easeing_.Start(easeingTime_);

			startEye_ = eye_;
			startTarget_ = target_;

			endEye_ = holdEye_;
			endTarget_ = holdTarget_;
		}
	}
	else if (phase_ == 1) {

		//取得したイージング用の開始位置と終了位置でイージングを行う
		eye_ = EaseIn3D(startEye_, endEye_, easeing_.timeRate);
		target_ = EaseIn3D(startTarget_, endTarget_, easeing_.timeRate);

		if (!easeing_.GetActive()) {
			//演出が終わったらモードの切り替え
			mode_ = PLAYERFOLLOWMODE;
			isPerformance_ = false;
		}
	}

	easeing_.Update();
}

void Camera::InitializeTitleToPlayMode()
{

	//イージング用の開始位置と終了位置を取得

	//現在の座標を開始位置に
	startEye_ = eye_;
	startTarget_ = target_;

	//初期座標を保持
	holdEye_ = startEye_;
	holdTarget_ = startTarget_;

	//現在の描画最大距離を終了位置に
	endEye_ = eye_;
	endTarget_ = target_;

	endEye_.z = eye_.z + farClip_;
	endTarget_.z = eye_.z + farClip_ + playerRange_;

	//イージング用数値の初期化
	easeing_.Start(easeingTime_);
	phase_ = 0;

	//パフォーマンスフラグ
	isPerformance_ = true;
}

void Camera::DebugMode()
{
	//カメラ操作
	float speed = 1.0f;

	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {

		//座標を移動する処理
		if (input_->PushKey(DIK_W)) {
			eye_.z += speed;
		}
		else if (input_->PushKey(DIK_S)) {
			eye_.z -= speed;
		}

		if (input_->PushKey(DIK_A)) {
			eye_.x -= speed;
		}
		else if (input_->PushKey(DIK_D)) {
			eye_.x += speed;
		}

	}

	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//座標を移動する処理
		if (input_->PushKey(DIK_UP)) {
			target_.y += speed;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			target_.y -= speed;
		}

		if (input_->PushKey(DIK_RIGHT)) {
			target_.x -= speed;
		}
		else if (input_->PushKey(DIK_LEFT)) {
			target_.x += speed;
		}

	}
}

void Camera::SetMode(int mode)
{
	this->mode_ = mode;

	//モードごとに初期化が必要な場合は初期化
	switch (mode_) {
	case TITLETOPLAYMODE:
		InitializeTitleToPlayMode();
		break;
	case STRAIGHTMODE:
		break;
	case BOSSINMODE:
		break;
	case BOSSCLERAMODE:
		break;
	case BOSSGAMEOVERAMODE:
		break;
	}
}
