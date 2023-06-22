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

void Camera::Initialize(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up, Input* input)
{
	//引数をメンバに移して行列更新
	this->eye = eye;
	this->target = target;
	this->up = up;
	this->input = input;



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

void Camera::Update()
{

	UpdateTarget();

	UpdateEye();

	UpdateMatrix();

}

void Camera::UpdateEye()
{
	//射影変換

	//if (input->PushKey(DIK_D) || input->PushKey(DIK_A)) {

	//	if (input->PushKey(DIK_D)) { angle += XMConvertToRadians(1.0f); }
	//	else if (input->PushKey(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

	//	//angleラジアンだけY軸回りに回転.半径は-100
	//	eye.x = -100 * sinf(angle);
	//	//eye.z = -10 * cosf(angle);

	//}

	if (input->PushKey(DIK_A)) {
		eye.x += 1;
	}
	if (input->PushKey(DIK_D)) {
		eye.x -= 1;
	}

	if (input->PushKey(DIK_W)) {
		eye.y += 1;
	}
	if (input->PushKey(DIK_S)) {
		eye.y -= 1;
	}

}

void Camera::UpdateTarget()
{

}