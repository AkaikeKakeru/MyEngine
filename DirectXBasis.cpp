#include "DirectXBasis.h"
#include <vector>
#include <string>
#include <cassert>

void DirectXBasis::Initialize() {
	InitDevice();
	InitCommand();
	InitSwapChain();
	InitRenderTargetView();
	InitDepthBuffer();
	InitFence();
}

void DirectXBasis::InitDevice() {
	HRESULT result;

#pragma region アダプタの列挙
	//DXGIファクトリ生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//アダプタの列挙用
	std::vector<IDXGIAdapter4*> adapters;
	//特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter = nullptr;

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


}

void DirectXBasis::InitCommand() {
}

void DirectXBasis::InitSwapChain() {
}

void DirectXBasis::InitRenderTargetView() {
}

void DirectXBasis::InitDepthBuffer() {
}

void DirectXBasis::InitFence() {
}

void DirectXBasis::Draw() {
}