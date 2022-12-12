#include "DirectXBasis.h"
#include <string>
#include <cassert>

void DirectXBasis::Initialize(WinApp* winApp) {
	assert(winApp);
	winApp_ = winApp;

	InitDevice();
	InitCommand();
	InitSwapChain();
	InitRenderTargetView();
	InitDepthBuffer();
	InitFence();
}

void DirectXBasis::InitDevice() {
	HRESULT result;

#ifdef _DEBUG
	//デバッグレイヤをオンに
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif

#pragma region アダプタの列挙
	//DXGIファクトリ生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//アダプタの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	//特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//パフォーマンスが高いものから順に、全てのアダプタを列挙
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		//動的配列に追加
		adapters.push_back(tmpAdapter);
	}

	//アダプタ選別
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプタ情報更新
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[1];
			break;
		}
	}
#pragma endregion

#pragma region デバイス生成
	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//採用したアダプタでデバイス生成
		result = D3D12CreateDevice(
			tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion

	//エラー発生時に強制ブレークを掛ける
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {

		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		infoQueue_->Release();
	}
}

void DirectXBasis::InitCommand() {
	HRESULT result;
#pragma region コマンドリスト
	//コマンドアロケータ生成
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator_));
	assert(SUCCEEDED(result));

	//コマンドリスト生成
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&cmdList_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region コマンドキュー
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	//コマンドキューの生成
	result = device_->CreateCommandQueue(
		&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue_));
	assert(SUCCEEDED(result));
#pragma endregion
}

void DirectXBasis::InitSwapChain() {
	HRESULT result;
	//スワップチェーンの設定
	swapChainDesc_.Width = winApp_->Win_Width;
	swapChainDesc_.Height = winApp_->Win_Height;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1のComPtr
	ComPtr<IDXGISwapChain1> swapChain1;
	//スワップチェーンの生成
	result = dxgiFactory_->CreateSwapChainForHwnd(
		cmdQueue_.Get(),
		winApp_->GetHWND(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);

	//IDXGISwapChain4に変換
	swapChain1.As(&swapChain_);
	assert(SUCCEEDED(result));
}

void DirectXBasis::InitRenderTargetView() {
	//レンダ―ターゲットビュー(RTV)は、デスクリプタヒープに生成する
#pragma region デスクリプタヒープ
	//デスクリプタヒープの設定
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount;

	//デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));
#pragma endregion

#pragma region レンダーターゲットビュー
	//バックバッファのリサイズ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		//スワップチェーンからバッファを取得
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
			rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		//裏か表かでアドレスがズレる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		//レンダ―ターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダ―ターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}
#pragma endregion
}

void DirectXBasis::InitDepthBuffer() {
}

void DirectXBasis::InitFence() {
	HRESULT result;
	//フェンスの生成
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;
	
	result = device_->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

void DirectXBasis::Draw() {
	//バックバッファの番号取得
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

#pragma region リソースバリアの変更コマンド
	//1.書き込み可能に
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = backBuffers_[bbIndex].Get();
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList_->ResourceBarrier(1, &barrierDesc);
#pragma endregion

#pragma region 描画先の変更指定コマンド
	//2.描画先の変更
	//レンダ―ターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
	cmdList_->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
#pragma endregion
}