#include "TitleScene.h"
#include "GamePlayScene.h"
#include "SafeDelete.h"

#include "Framework.h"

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();

void TitleScene::Initialize() {
	isStart_ = false;

	/// 描画初期化
	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get(), WinApp::Win_Width, WinApp::Win_Height);

	//オブジェクトモデル
	//天球
	modelSkydome_ = Model::LoadFromOBJ("skydome");
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);
	skydome_->Update();

	//自機
	modelPlayer_ = Model::LoadFromOBJ("plane");
	player_ = new Player();
	player_->Initialize(modelPlayer_);
	player_->Update();

	//描画基盤
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "title.png");
	drawBas_->LoadTexture(1, "press.png");
	drawBas_->LoadTexture(2, "back.png");
	drawBas_->LoadTexture(3, "over.png");

	//描画スプライト
	title_ = new Sprite();
	back_ = new Sprite();
	ui_ = new Sprite();

	title_->Initialize(drawBas_, 0);
	back_->Initialize(drawBas_, 2);
	ui_->Initialize(drawBas_, 1);

	back_->SetAnchorPoint({ 0.5f,0.5f });
	back_->SetPosition({ WinApp::Win_Width / 2,WinApp::Win_Height / 2 });
	back_->Update();

	ui_->SetAnchorPoint({ 0.5f,0.5f });
	ui_->SetPosition({ WinApp::Win_Width / 2 + 260.0f,WinApp::Win_Height / 2 + 160.0f });
	ui_->Update();

	title_->SetAnchorPoint({ 0.5f,0.5f });
	title_->SetPosition({ WinApp::Win_Width / 2,WinApp::Win_Height / 2 - 150.0f });
	title_->Update();
}

void TitleScene::Update() {
	input_->Update();

	if (input_->PressKey(DIK_RETURN)) {
		isStart_ = true;
	}
}

void TitleScene::Draw() {
	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	skydome_->Draw();

	player_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	if (!isStart_) {
		back_->Draw();
		ui_->Draw();
		title_->Draw();
	}

	drawBas_->PostDraw();
}

void TitleScene::Finalize() {
	SafeDelete(ui_);
	SafeDelete(back_);
	SafeDelete(title_);

	SafeDelete(skydome_);
	SafeDelete(player_);

	SafeDelete(modelSkydome_);
	SafeDelete(modelPlayer_);
}