#include "GameMain.h"
#include "TitleScene.h"
#include "GamePlayScene.h"

Input* GameMain::input_ = Input::GetInstance();

void GameMain::Initialize() {

	FrameworkΓ::Initialize();

	scene_ = new TitleScene();

	scene_->Initialize();
}

void GameMain::Update() {
	FrameworkΓ::Update();

	if (sceneNum != GamePlay_scene) {
		if (input_->TriggerKey(DIK_RETURN)) {

			scene_->Finalize();
			delete scene_;

			switch (sceneNum) {
			case Title_scene:
				sceneNum = GamePlay_scene;
				scene_ = new GamePlayScene();
				break;

			case GamePlay_scene:
				break;
			default:
				break;
			}

			scene_->Initialize();
		}
	}

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
	FrameworkΓ::Finalize();
}