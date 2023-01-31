#include "Framework.h"
#include "Object3d.h"
#include "DrawBasis.h"
#include "TitleScene.h"

SceneManager* FrameworkΓ::sceneManager_ = SceneManager::GetInstance();

void FrameworkΓ::Run(){
	/// 初期化
	Initialize();

	/// ゲームループ
	while (true) {
		/// 更新
		Update();

		if (IsEndRequest()) {
			//終了リクエストが来たら抜ける
			break;
		}

		/// 描画
		Draw();
	}

	Finalize();
}

void FrameworkΓ::Initialize(){
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

	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get());

	DrawBasis::GetInstance();
	DrawBasis::Initialize();

}

void FrameworkΓ::Update(){
	//windowsのメッセージ処理
	if (winApp_->ProcessMessage()) {
		//ゲームループを抜ける
		isEndRequest_ = true;
	}

	sceneManager_->Update();
}

void FrameworkΓ::Finalize(){
	sceneManager_->Finalize();
	delete sceneFactory_;
}