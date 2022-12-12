#include <Windows.h>
#include "SafeDelete.h"
#include "WinApp.h"
#include "DirectXBasis.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	///基盤初期化
	//アプリケーション
	WinApp* winApp_ = new WinApp();
	winApp_->Initialize();

	//DirectX基盤
	DirectXBasis* dxBas_ = new DirectXBasis();
	dxBas_->Initialize(winApp_);

	/// 描画初期化

	/// ゲームループ
	while (true) {
		//windowsのメッセージ処理
		if (winApp_->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}

		/// 更新

		/// 描画
		dxBas_->Draw();
	}

	/// 解放
	SafeDelete(dxBas_);
	SafeDelete(winApp_);

	return 0;
}