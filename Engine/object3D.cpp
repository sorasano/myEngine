#include "object3D.h"

//3Dオブジェクト初期化
void InitializeObject3d(Object3d2* object, ComPtr<ID3D12Device> device_)
{
	HRESULT result;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform_)
	);
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = object->constBuffTransform_->Map(0, nullptr, (void**)&object->constMapTransform_);
	assert(SUCCEEDED(result));
}

void UpdateObject3d(Object3d2* object, XMMATRIX& matView_, XMMATRIX& matProjection_)
{
	XMMATRIX matScale, matRot, matTrans;

	matScale = XMMatrixScaling(object->scale_.x, object->scale_.y, object->scale_.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation_.z);
	matRot *= XMMatrixRotationX(object->rotation_.x);
	matRot *= XMMatrixRotationY(object->rotation_.y);

	matTrans = XMMatrixTranslation(object->position_.x, object->position_.y, object->position_.z);

	//ワールド行列の合成
	object->matWorld_ = XMMatrixIdentity();
	object->matWorld_ *= matScale;
	object->matWorld_ *= matRot;
	object->matWorld_ *= matTrans;

	//親オブジェクトがあれば
	if (object->parent != nullptr)
	{
		object->matWorld_ *= object->parent->matWorld_;
	}

	//定数バッファへデータ転送
	object->constMapTransform_->mat = object->matWorld_ * matView_ * matProjection_;
}

//オブジェクト描画処理
void DrawObject3d(Object3d2* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView_,
	D3D12_INDEX_BUFFER_VIEW& ibView_, UINT numIndices)
{
	//頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform_->GetGPUVirtualAddress());
	//インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView_);
	//描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);	//全ての頂点を使って描画
}

void Object3D::Initialize(DirectXCommon* dx, Model* model)
{
	this->dx_ = dx;
	this->model_ = model;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);

	HRESULT result;

	// 定数バッファの生成
	result = this->dx_->GetDevice()->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));

	// リソース設定
	resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = this->dx_->GetDevice()->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffB1));
	assert(SUCCEEDED(result));
}

void Object3D::Update(XMMATRIX& matView, XMMATRIX& matProjection)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation_.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation_.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation_.y));
	matTrans = XMMatrixTranslation(position_.x, position_.y, position_.z);

	// ワールド行列の合成
	matWorld_ = XMMatrixIdentity(); // 変形をリセット
	matWorld_ *= matScale; // ワールド行列にスケーリングを反映
	matWorld_ *= matRot; // ワールド行列に回転を反映
	matWorld_ *= matTrans; // ワールド行列に平行移動を反映

	//// 親オブジェクトがあれば
	//if (parent != nullptr) {
	//	// 親オブジェクトのワールド行列を掛ける
	//	matWorld *= parent->matWorld;
	//}

	// 定数バッファへデータ転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->mat = matWorld_ * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);

	//定数バッファへデータ転送
	ConstBufferDataB1* constMap1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	constMap1->ambient = model_->GetMaterial().ambient;
	constMap1->diffuse = model_->GetMaterial().diffuse;
	constMap1->specular = model_->GetMaterial().specular;
	constMap1->alpha = model_->GetMaterial().alpha;
	constBuffB1->Unmap(0, nullptr);
}

void Object3D::Draw(D3D12_VERTEX_BUFFER_VIEW& vbView,D3D12_INDEX_BUFFER_VIEW& ibView)
{
	this->cmdList_ = dx_->GetCommandList();
	// パイプラインステートの設定
	cmdList_->SetPipelineState(model_->GetPipelinestate().Get());
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(model_->GetRootSignature().Get());
	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { model_->GetDescHeap().Get()};
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = model_->GetSrv();
	UINT incrementSize = dx_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvGpuHandle.ptr += incrementSize * model_->textureIndex;
	// シェーダリソースビューをセット
	cmdList_->SetGraphicsRootDescriptorTable(2, srvGpuHandle);
	// 描画コマンド
	cmdList_->DrawIndexedInstanced(model_->GetIndicesSize(), 1, 0, 0, 0);
}

void Object3D::setPosition(XMFLOAT3 pos)
{
	position_ = pos;
}

void Object3D::setRotation(XMFLOAT3 rot)
{
	rotation_ = rot;
}

void Object3D::setScale(XMFLOAT3 sca)
{
	scale_ = sca;
}

Object3D* Object3D::GetInstance()
{
	static Object3D instance;
	return &instance;
}

Object3D::Object3D()
{
}

Object3D::~Object3D()
{
}
