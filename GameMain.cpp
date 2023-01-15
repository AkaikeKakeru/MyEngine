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
	if (input_->TriggerKey(DIK_RETURN)) {
		switch (sceneNum) {
		case Title_scene:
			sceneNum = Wait_scene;
			break;

		case Wait_scene:
			break;

		case GamePlay_scene:
			sceneNum = Wait_scene;
			break;

		default:
			break;
		}
	}

	if (sceneNum == Wait_scene) {
		startTimer_--;

		if (startTimer_ <= 0) {
			sceneNum = GamePlay_scene;
			startTimer_ = kStartInterval;

			scene_->Finalize();
			delete scene_;

			scene_ = new GamePlayScene();
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
	delete scene_;
	FrameworkΓ::Finalize();
}