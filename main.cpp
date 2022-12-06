#include <Windows.h>
#include "WinApp.h"
#include "SafeDelete.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//基盤初期化
	WinApp* winApp_ = new WinApp();
	winApp_->Initialize();

	//描画初期化

	//ゲームループ
	while (true) {
		//windowsのメッセージ処理
		if (winApp_->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}

		//更新

		//描画
	}

	//解放
	SafeDelete(winApp_);

	return 0;
}