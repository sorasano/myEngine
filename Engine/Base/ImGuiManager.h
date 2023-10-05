#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class ImGuiManager
{
public:
	//������
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	//�I�����Ăяo������
	void Finalize();
	//ImGui��t�J�n
	void Begin();
	//ImGui��t�I��
	void End();
	//�`��
	void Draw();


private:
	WinApp* winApp_;
	DirectXCommon* dxCommon_;

	//SRV�p�f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>srvHeap_;
};

