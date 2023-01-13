#include "GameMain.h"

void GameMain::Run(){
	/// 初期化
	Initialize();

	/// ゲームループ
	while (true) {
		/// 更新
		Update();

		//windowsのメッセージ処理
		if (winApp_->ProcessMessage()) {
			//ゲームループを抜ける
			//isEndRequest_ = true;
			break;
		}

		/// 描画
		Draw();
	}

	Finalize();
}

void GameMain::Initialize(){
	///基盤初期化
	//アプリケーション
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize();

	//DirectX基盤
	dxBas_ = DirectXBasis::GetInstance();
	dxBas_->Initialize(winApp_);

	//Input
	input_ = Input::GetInstance();
	input_->Initialize();

	//シーン
	//scene_ = new GamePlayScene();
	scene_ = new TitleScene();
	scene_->Initialize();
}

void GameMain::Update(){

	/// 更新
	scene_->Update();
}

void GameMain::Draw(){
	//描画前処理
	dxBas_->PreDraw();

	scene_->Draw();

	//描画後処理
	dxBas_->PostDraw();
}

void GameMain::Finalize(){
	/// 解放
	scene_->Finalize();

	delete scene_;
}