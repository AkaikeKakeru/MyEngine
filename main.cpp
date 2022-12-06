#include <Windows.h>
#include "WinApp.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//基盤初期化
	WinApp* winApp_ = new WinApp();
	winApp_->Initialize();

	//描画初期化

	//ゲームループ
	while (true) {

		//更新

		//描画
	}

	//解放
	delete winApp_;
	winApp_ = nullptr;

	return 0;
}