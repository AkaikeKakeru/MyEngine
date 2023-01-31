#include "TitleScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();

void TitleScene::Initialize(){
	/// 描画初期化
	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get());

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

	if (input_->TriggerKey(DIK_RETURN)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
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