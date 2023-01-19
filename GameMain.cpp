#include "GameMain.h"
#include "GamePlayScene.h"

Input* GameMain::input_ = Input::GetInstance();

void GameMain::Initialize() {
	FrameworkΓ::Initialize();
	scene_ = new GamePlayScene();
	scene_->Initialize();
}

void GameMain::Update() {
	FrameworkΓ::Update();

	scene_->Update();
}

void GameMain::Draw() {
	//描画前処理
	FrameworkΓ::GetDirectXBasis()->PreDraw();
	scene_->Draw();
	//描画後処理
	FrameworkΓ::GetDirectXBasis()->PostDraw();
}

void GameMain::Finalize() {
	scene_->Finalize();
	delete scene_;
	FrameworkΓ::Finalize();
}