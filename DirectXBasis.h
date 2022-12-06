#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DirectXBasis {
public://static
public://基本関数	
	void Initialize();
	void Draw();

public://固有関数
	///初期化
	//デバイス
	void InitDevice();
	//コマンド
	void InitCommand();
	//スワップチェーン
	void InitSwapChain();
	//レンダ―ターゲットビュー
	void InitRenderTargetView();
	//深度バッファ
	void InitDepthBuffer();
	//フェンス
	void InitFence();

public://定数
private://変数
	ID3D12Device* device_;
	IDXGIFactory7* dxgiFactory_;
	IDXGISwapChain4* swapChain_;
	ID3D12CommandAllocator* cmdAllocator_;
	ID3D12GraphicsCommandList* cmdList_;
	ID3D12CommandQueue* cmdQueue_;
	ID3D12DescriptorHeap* rtvHeap_;
};