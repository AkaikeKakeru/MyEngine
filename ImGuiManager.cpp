#include "ImGuiManager.h"
#include <imgui_impl_win32.h>
#include "WinApp.h"

void ImGuiManager::Initialize() {
	//コンテキストを生成
	ImGui::CreateContext();
	//スタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());
}