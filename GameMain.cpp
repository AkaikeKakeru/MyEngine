#include "GameMain.h"
#include "SceneFactory.h"

SceneManager* GameMain::sceneManager_ = FrameworkΓ::GetSceneManager();

void GameMain::Initialize() {
	FrameworkΓ::Initialize();

	sceneManager_ = FrameworkΓ::GetSceneManager();

	//シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");
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