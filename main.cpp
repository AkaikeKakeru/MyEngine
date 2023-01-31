#include "Framework.h"
#include "GameMain.h"
#include "ImGuiManager.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	ImGuiManager* imGuiManager_ = new ImGuiManager();
	imGuiManager_->Initialize();

	FrameworkΓ* game = new GameMain();
	game->Run();

	delete imGuiManager_;
	delete game;
	return 0;
}