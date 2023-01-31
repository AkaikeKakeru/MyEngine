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
	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", false);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome",false);


	planeObj_ = new Object3d();
	planeObj_ = Object3d::Create();
	planeObj_->SetModel(planeModel_);

	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
}

void GamePlayScene::Initialize2d() {
	drawBas_->LoadTexture(1, "texture.png");
	sprite_->Initialize(drawBas_,1);
}

void GamePlayScene::Update3d() {
	skydomeObj_->Update();
	planeObj_->Update();
}

void GamePlayScene::Update2d() {
	sprite_->Update();
}

void GamePlayScene::Draw3d() {
	skydomeObj_->Draw();
	planeObj_->Draw();
}

void GamePlayScene::Draw2d() {
	sprite_->Draw();
}

void GamePlayScene::Finalize() {
	SafeDelete(planeObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);
}
