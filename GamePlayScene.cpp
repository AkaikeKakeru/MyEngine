#include "GamePlayScene.h"
#include "SafeDelete.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();

void GamePlayScene::Initialize() {
	Initialize2d();
	Initialize3d();
}

void GamePlayScene::Update() {
	input_->Update();

	Update3d();
	Update2d();
}

void GamePlayScene::Draw() {
	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	Draw3d();
	Object3d::PostDraw();

	drawBas_->PreDraw();
	Draw2d();
	drawBas_->PostDraw();
}

void GamePlayScene::Initialize3d() {
}

void GamePlayScene::Initialize2d() {
	drawBas_->LoadTexture(1, "texture.png");
	sprite_->Initialize(drawBas_,1);
}

void GamePlayScene::Update3d() {
}

void GamePlayScene::Update2d() {
	sprite_->Update();
}

void GamePlayScene::Draw3d() {
}

void GamePlayScene::Draw2d() {
	sprite_->Draw();
}

void GamePlayScene::Finalize() {
	SafeDelete(sprite_);
}