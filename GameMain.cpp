#include "GameMain.h"
#include "TitleScene.h"
#include "GamePlayScene.h"

SceneManager* GameMain::sceneManager_ = FrameworkΓ::GetSceneManager();

void GameMain::Initialize() {
	FrameworkΓ::Initialize();

	sceneManager_ = FrameworkΓ::GetSceneManager();

	BaseScene* scene = new TitleScene();

	sceneManager_->SetNextScene(scene);
}

void GameMain::Update() {
	FrameworkΓ::Update();
}

void GameMain::Draw() {
	//描画前処理
	FrameworkΓ::GetDirectXBasis()->PreDraw();

	sceneManager_ = FrameworkΓ::GetSceneManager();
	sceneManager_->Draw();

	//描画後処理
	FrameworkΓ::GetDirectXBasis()->PostDraw();
}

void GameMain::Finalize() {
	FrameworkΓ::Finalize();
}