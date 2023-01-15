#include "TitleScene.h"
#include "SafeDelete.h"

//#include "SceneManager.h"
#include "GamePlayScene.h"

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();

void TitleScene::Initialize(){
	/// 描画初期化
	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get(), WinApp::Win_Width, WinApp::Win_Height);

	//オブジェクトモデル

	//描画基盤
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "HoppingTitle.png");

	//描画スプライト

	sprite_->Initialize(drawBas_,0);
}

void TitleScene::Update(){
	input_->Update();
	sprite_->Update();

	//if (input_->TriggerKey(DIK_RETURN)) {
	//	//最初のシーン
	//	BaseScene* scene_ = new GamePlayScene();

	//	//シーンマネージャーにセット
	//	sceneManager_->SetNextScene(scene_);
	//}
}

void TitleScene::Draw(){
	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	sprite_->Draw();

	drawBas_->PostDraw();
}

void TitleScene::Finalize(){
	SafeDelete(sprite_);
}