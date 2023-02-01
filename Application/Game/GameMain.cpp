#include "GameMain.h"
#include "SceneFactory.h"

SceneManager* GameMain::sceneManager_ = Framework_KA::GetSceneManager();

void GameMain::Initialize() {
	Framework_KA::Initialize();

	sceneManager_ = Framework_KA::GetSceneManager();

	//シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
}

void GameMain::Update() {
	Framework_KA::Update();
}

void GameMain::Draw() {
	//描画前処理
	Framework_KA::GetDirectXBasis()->PreDraw();

	sceneManager_ = Framework_KA::GetSceneManager();
	sceneManager_->Draw();

	//描画後処理
	Framework_KA::GetDirectXBasis()->PostDraw();
}

void GameMain::Finalize() {
	Framework_KA::Finalize();
}