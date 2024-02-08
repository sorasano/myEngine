/**
* @file ImGuiManager.cpp
* @brief ImGui
*/

#include "ImGuiManager.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

void ImGuiManager::Initialize()
{
	this->winApp_ = MyEngine::WinApp::GetInstance();;
	this->dxCommon_ = MyEngine::DirectXCommon::GetInstance();

	HRESULT result;

	//ImGuiのコンテキストを作成
	ImGui::CreateContext();
	//ImGuiのスタイルを作成
	ImGui::StyleColorsDark();

	//Win32用初期化関数
	ImGui_ImplWin32_Init(winApp_->GetHwnd());

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//デスクリプタヒープ生成
	result = dxCommon_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	//DirectX12用初期化処理
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);

	//標準フォントを追加する
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Finalize()
{
	//後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//デスクリプタヒープを解放
	srvHeap_.Reset();
}

void ImGuiManager::Begin()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	ImGui::Render();
}

void ImGuiManager::Draw()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}
