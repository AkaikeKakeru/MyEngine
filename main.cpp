#include <Windows.h>
#include "SafeDelete.h"
#include "WinApp.h"
#include "DirectXBasis.h"
#include "DrawBasis.h"
#include "Sprite.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	///基盤初期化
	//アプリケーション
	WinApp* winApp_ = nullptr;
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize();

	//DirectX基盤
	DirectXBasis* dxBas_ = nullptr;
	dxBas_ = DirectXBasis::GetInstance();
	dxBas_->Initialize(winApp_);

	/// 描画初期化

	//描画基盤
	DrawBasis* drawBas_ = nullptr;
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	//描画スプライト
	Sprite* sprite_ = new Sprite();
	sprite_->Initialize(drawBas_);

	Sprite* sprite2_ = new Sprite();
	sprite2_->Initialize(drawBas_);

	/// ゲームループ
	while (true) {
		//windowsのメッセージ処理
		if (winApp_->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}

		/// 更新
		Vector2 pos = { 0,0 };

		pos = sprite_->GetPosition();
		pos.x += 2.0f;
		sprite_->SetPosition(pos);
		sprite_->SetColor(Vector4(0.5f, 1.0f, 0.2f, 0.4f));
		sprite_->Update();

		pos = sprite2_->GetPosition();
		pos.y += 2.0f;
		sprite2_->SetPosition(pos);
		sprite2_->SetColor(Vector4(0.8f, 0.2f, 0.5f, 0.9f));
		sprite2_->Update();

		/// 描画
		//描画前処理
		dxBas_->PreDraw();

		//描画本命処理
		drawBas_->Draw();
		sprite_->Draw();
		sprite2_->Draw();

		//描画後処理
		dxBas_->PostDraw();
	}

	/// 解放
	SafeDelete(sprite_);
	SafeDelete(sprite2_);

	//SafeDelete(drawBas_);
	//SafeDelete(dxBas_);
	//SafeDelete(winApp_);

	return 0;
}