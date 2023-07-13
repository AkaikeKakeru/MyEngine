#include "GameMain.h"
#include "SceneFactory.h"

SceneManager* GameMain::sceneManager_ = Framework::GetSceneManager();

void GameMain::Initialize() {
	Framework::Initialize();

	sceneManager_ = Framework::GetSceneManager();

	//シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");
}

void GameMain::Update() {
	Framework::Update();
}

void GameMain::Draw() {
	//描画前処理
	Framework::GetDirectXBasis()->PreDraw();

	//スプライト描画事前処理
	DrawBasis::GetInstance()->PreDraw();

	//ポストエフェクト描画
	Framework::GetPostEffect()->Draw();
	
	//スプライト描画事後処理
	DrawBasis::GetInstance()->PostDraw();

	//シーンマネージャー取得
	sceneManager_ = Framework::GetSceneManager();
	//シーンマネージャーから、シーンを描画
	sceneManager_->Draw();

	//描画後処理
	Framework::GetDirectXBasis()->PostDraw();
}

void GameMain::Finalize() {
	Framework::Finalize();
}