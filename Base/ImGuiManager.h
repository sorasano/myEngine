#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class ImGuiManager
{
public:
	//初期化
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	//終了時呼び出し処理
	void Finalize();
	//ImGui受付開始
	void Begin();
	//ImGui受付終了
	void End();
	//描画
	void Draw();


private:
	WinApp* winApp_;
	DirectXCommon* dxCommon_;

	//SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>srvHeap_;
};

