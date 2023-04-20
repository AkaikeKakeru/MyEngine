#include "Framework.h"
#include "GameMain.h"
#include "fbxsdk.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	FbxManager* fbxManager = FbxManager::Create();

	Framework* game = new GameMain();
	game->Run();

	delete game;
	return 0;
}