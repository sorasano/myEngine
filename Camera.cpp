#include "Camera.h"
#include"Base/WinApp.h"
using namespace DirectX;

ID3D12Device* Camera::device = nullptr;

void Camera::StaticInitialize(ID3D12Device* dev)
{
	//NULLチェック
	assert(dev);
	device = dev;
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
	result = device->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = constBuff->Map(0, nullptr, (void**)&constMap);//マッピング
	assert(SUCCEEDED(result));

	UpdateMatrix();
}

void Camera::UpdateMatrix()
{
	//専用の行列を宣言
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),					//上下画角45度
		(float)WinApp::winW / WinApp::winH,	//アスペクト比（画面横幅/画面縦幅）
		0.1f, 1000.0f								//前橋、奥橋
	);

	//ビュー変換行列の計算
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	constMap->view = matView;
	constMap->projection = matProjection;
}

void Camera::Update(XMFLOAT3 playerPos)
{

	this->playerPos_ = playerPos;

	UpdateTarget();

	UpdateEye();

	//DebugMode();

	UpdateMatrix();

}

void Camera::UpdateEye()
{
	eye.z = playerPos_.z - 30;
}

void Camera::UpdateTarget()
{
	target.z = playerPos_.z;
}

void Camera::DebugMode()
{

	float speed = 1.0f;

	if (input_->PushKey(DIK_W) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {

		//座標を移動する処理
		if (input_->PushKey(DIK_W)) {
			 eye.z += speed;
		}
		else if (input_->PushKey(DIK_S)) {
			eye.z -= speed;
		}

		if (input_->PushKey(DIK_A)) {
			eye.x -= speed;
		}
		else if (input_->PushKey(DIK_D)) {
			eye.x += speed;
		}

	}

	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT)) {

		//座標を移動する処理
		if (input_->PushKey(DIK_UP)) {
			target.y += speed;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			target.y -= speed;
		}

		if (input_->PushKey(DIK_RIGHT)) {
			target.x -= speed;
		}
		else if (input_->PushKey(DIK_LEFT)) {
			target.x += speed;
		}

	}
}
