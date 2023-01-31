#include "ImGuiManager.h"
#include <imgui_impl_win32.h>
#include <imGui_impl_DX12.h>
#include "WinApp.h"

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(DirectXBasis* dXBas) {
	HRESULT result;

	//コンテキストを生成
	ImGui::CreateContext();
	//スタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//デスクリプタヒープ生成
	result = dXBas->GetDevice()->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	ImGui_ImplDX12_Init(
		dXBas->GetDevice().Get(),
		static_cast<int>(dXBas->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);
}
