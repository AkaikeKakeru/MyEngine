#include "GameMain.h"
#include "TitleScene.h"

void GameMain::Initialize(){

	FrameworkΓ::Initialize();
	sceneManager_ = FrameworkΓ::GetSceneManager();

	BaseScene* scene = new TitleScene();
	sceneManager_->SetNextScene(scene);
}

void GameMain::Update(){
	FrameworkΓ::Update();
}

void GameMain::Draw(){
	//描画前処理
	FrameworkΓ::GetDirectXBasis()->PreDraw();

	sceneManager_->Draw();

	//描画後処理
	FrameworkΓ::GetDirectXBasis()->PostDraw();
}

void GameMain::Finalize(){

	//scene_->Finalize();
	FrameworkΓ::Finalize();
}