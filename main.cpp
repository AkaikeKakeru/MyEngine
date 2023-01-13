#include "GameMain.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	GameMain game;

	/// 初期化
	game.Initialize();

	/// ゲームループ
	while (true) {
		/// 更新
		game.Update();

		if (game.IsEndRequest()) {
			break;
		}

		/// 描画
		game.Draw();
	}

	game.Finalize();

	return 0;
}