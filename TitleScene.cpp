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
	//modelPlayer_ = Model::LoadFromOBJ("plane");
	//player_ = new Player();
	//player_->Initialize(drawBas_,modelPlayer_);
	//player_->Update();

	//描画基盤
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "title.png");
	drawBas_->LoadTexture(1, "press.png");
	drawBas_->LoadTexture(2, "back.png");
	drawBas_->LoadTexture(3, "over.png");
	drawBas_->LoadTexture(4, "texture.png");

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

	backStartPos_ = {
		back_->GetPosition().x,
		back_->GetPosition().y,
		0
	};

	backEndPos_ = {
		back_->GetPosition().x,
		-500.0f,
		0
	};

	ui_->SetAnchorPoint({ 0.5f,0.5f });
	ui_->SetPosition({ WinApp::Win_Width / 2 + 260.0f,WinApp::Win_Height / 2 + 160.0f });
	ui_->Update();

	uiStartPos_ = {
		ui_->GetPosition().x,
		ui_->GetPosition().y,
		0
	};

	uiEndPos_ = {
		ui_->GetPosition().x,
		900.0f,
		0
	};

	title_->SetAnchorPoint({ 0.5f,0.5f });
	title_->SetPosition({ WinApp::Win_Width / 2,WinApp::Win_Height / 2 - 150.0f });
	title_->Update();

	titleStartPos_ = {
		title_->GetPosition().x,
		title_->GetPosition().y,
		0
	};

	titleEndPos_ = {
		title_->GetPosition().x,
		-200.0f,
		0
	};
}

void TitleScene::Update() {
	input_->Update();

	if (input_->PressKey(DIK_RETURN)) {
		isStart_ = true;
	}

	if (isStart_) {

		count_ += 0.1f;

		timeRate_ = min(count_ / MaxTime_, 1.0f);

		Vector3 posTitle = EaseOut(titleStartPos_, titleEndPos_, timeRate_);
		Vector3 posBack = EaseIn(backStartPos_, backEndPos_, timeRate_);
		Vector3 posUi = EaseIn(uiStartPos_, uiEndPos_, timeRate_);

		back_->SetPosition({ posBack.x, posBack.y });
		back_->Update();
		ui_->SetPosition({ posUi.x, posUi.y });
		ui_->Update();
		title_->SetPosition({ posTitle.x, posTitle.y });
		title_->Update();
	}
}

void TitleScene::Draw() {
	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	skydome_->Draw();

	//player_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	back_->Draw();
	ui_->Draw();
	title_->Draw();

	drawBas_->PostDraw();
}

void TitleScene::Finalize() {
	SafeDelete(ui_);
	SafeDelete(back_);
	SafeDelete(title_);

	SafeDelete(skydome_);
	//SafeDelete(player_);

	SafeDelete(modelSkydome_);
	SafeDelete(modelPlayer_);
}