#pragma once
#include <Windows.h>
#include <cstdlib>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <algorithm>
#include <cassert>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "WinApp.h"

using namespace Microsoft::WRL;

class DirectXCommon
{
public:
	//�V���O���g���C���X�^���X���擾
	static DirectXCommon* GetInstance();
	DirectXCommon() = default;
	~DirectXCommon();
	//����������
	void Initialize(WinApp* winApp);
	void InitializeDevice();			//�f�o�C�X�֘A
	void InitializeCommand();			//�R�}���h�֘A
	void InitializeSwapchain();			//�X���b�v�`�F�[���֘A
	void InitializeRenderTargetView();	//�����_�[�^�[�Q�b�g�֘A
	void InitializeDepthBuffer();		//�[�x�o�b�t�@�֘A
	void InitializeFence();				//�t�F���X�֘A
	//�`��
	void PreDraw();		//�`��O
	void PostDraw();	//�`��� 
	//imgui�p�̃q�[�v����
	ComPtr<ID3D12DescriptorHeap>CreateDescriptorForImgui();
	void InitializeImgui();

	//�Q�b�^�[
	ID3D12Device* GetDevice() { return device.Get(); }
	IDXGISwapChain4* GetSwapChain() { return swapChain.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
	ID3D12Debug* GetDebugController() { return debugController.Get(); }
	IDXGIFactory7* GetDxgiFactory() { return dxgiFactory.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator.Get(); }
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue.Get(); }
	ID3D12DescriptorHeap* GetRtvHeap() { return rtvHeap.Get(); }
	ID3D12Fence* GetFence() { return fence.Get(); }
	ComPtr<ID3D12DescriptorHeap>GetHeapForImgui() { return _heapForImgui; };

	//�o�b�N�o�b�t�@�̐����擾
	size_t GetBackBufferCount() const { return backBuffers.size(); }
public:
	//�����o�ϐ�
	//�E�B���h�E
	WinApp* winApp_;
private:
	//DirectX
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Debug> debugController;
	ComPtr<IDXGIFactory7> dxgiFactory;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Fence> fence;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> depthBuff;
	D3D12_RESOURCE_DESC depthResorceDesc{};
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	D3D12_CLEAR_VALUE depthClearValue{};
	D3D12_RESOURCE_BARRIER barrierDesc{};
	//imgui�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _heapForImgui;
public:
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	std::vector<ComPtr<ID3D12Resource>>backBuffers;
	/*std::vector<ComPtr<IDXGIAdapter4>>adapters;
	ComPtr<IDXGIAdapter4> tmpAdapter;*/

	UINT64 fenceVal = 0;
};

