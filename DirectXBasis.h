#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DirectXBasis {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://static
public://基本関数	
	void Initialize(WinApp * winApp);
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
	WinApp* winApp_ = nullptr;

	//デバッグレイヤー
	ID3D12Debug1* debugController_{};
	//自動ブレーク
	ID3D12InfoQueue* infoQueue_{};

	ComPtr<ID3D12Device> device_;
	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGISwapChain4> swapChain_;
	ComPtr<ID3D12CommandAllocator> cmdAllocator_;
	ComPtr<ID3D12GraphicsCommandList> cmdList_;
	ComPtr<ID3D12CommandQueue> cmdQueue_;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_;
};