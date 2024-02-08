/**
* @file ImGuiManager.h
* @brief ImGui
*/

#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class ImGuiManager
{
public:
	/**
	* 初期化
	*/
	void Initialize();
	/**
	* 終了時呼び出し処理
	*/
	void Finalize();
	/**
	* ImGui受付開始
	*/
	void Begin();
	/**
	* ImGui受付終了
	*/
	void End();
	/**
	* 描画
	*/

	void Draw();

private:
	MyEngine::WinApp* winApp_;
	MyEngine::DirectXCommon* dxCommon_;

	//SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>srvHeap_;
};

