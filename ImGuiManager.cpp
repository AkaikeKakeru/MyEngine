#include "ImGuiManager.h"
#include <imgui_impl_win32.h>
#include "WinApp.h"

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(DirectXBasis* dXBas) {
	//コンテキストを生成
	ImGui::CreateContext();
	//スタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());
}
