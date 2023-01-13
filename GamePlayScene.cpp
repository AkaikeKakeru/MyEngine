#include "GamePlayScene.h"
#include "SafeDelete.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();

void GamePlayScene::Initialize(){
	/// 描画初期化
	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get(), WinApp::Win_Width, WinApp::Win_Height);

	//オブジェクトモデル

	model_ = Model::LoadFromOBJ("cube");
	model_2 = Model::LoadFromOBJ("triangle_mat");
	modelSkydome_ = Model::LoadFromOBJ("skydome");
	modelGround_ = Model::LoadFromOBJ("ground");

	object3d_ = Object3d::Create();
	object3d_2 = Object3d::Create();
	skydome_ = Object3d::Create();
	ground_ = Object3d::Create();

	object3d_->SetModel(model_);
	object3d_2->SetModel(model_2);

	skydome_->SetModel(modelSkydome_);
	ground_->SetModel(modelGround_);

	object3d_->SetPosition({ -5.0f, 0.0f, -5.0f });
	object3d_2->SetPosition({ +5.0f, 0.0f, +5.0f });

	//描画基盤
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "smile.png");
	drawBas_->LoadTexture(1, "texture.png");

	//描画スプライト

	sprite_->Initialize(drawBas_,0);
	sprite2_->Initialize(drawBas_,0);
	sprite3_->Initialize(drawBas_,1);
}

void GamePlayScene::Update(){
	input_->Update();

	Vector2 pos = { 0,0 };

	//オブジェクト更新

	//オブジェクト移動
	if (input_->PressKey(DIK_UP) || input_->PressKey(DIK_DOWN) || input_->PressKey(DIK_RIGHT) || input_->PressKey(DIK_LEFT)) {
		// 現在の座標を取得
		Vector3 position = object3d_->GetPosition();

		// 移動後の座標を計算
		if (input_->PressKey(DIK_UP)) { position.y += 1.0f; }
		else if (input_->PressKey(DIK_DOWN)) { position.y -= 1.0f; }
		if (input_->PressKey(DIK_RIGHT)) { position.x += 1.0f; }
		else if (input_->PressKey(DIK_LEFT)) { position.x -= 1.0f; }

		// 座標の変更を反映
		object3d_->SetPosition(position);
	}

	// カメラ移動
	if (input_->PressKey(DIK_W) || input_->PressKey(DIK_S) || input_->PressKey(DIK_D) || input_->PressKey(DIK_A)) {
		if (input_->PressKey(DIK_W)) { Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f }); }
		else if (input_->PressKey(DIK_S)) { Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f }); }
		if (input_->PressKey(DIK_D)) { Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f }); }
		else if (input_->PressKey(DIK_A)) { Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f }); }
	}

	skydome_->Update();
	ground_->Update();
	object3d_->Update();
	object3d_2->Update();

	pos = sprite_->GetPosition();
	if (input_->PressMouse(LeftButton)) {
		pos.x += 2.0f;
	}
	sprite_->SetPosition(pos);
	sprite_->SetRotation(ConvertToRadian(45.0f));
	sprite_->SetColor(Vector4(0.5f, 1.0f, 0.2f, 0.4f));
	sprite_->SetSize(Vector2(80, 120));
	sprite_->SetIsFlipX(true);
	sprite_->Update();

	pos = sprite2_->GetPosition();
	if (input_->TriggerMouse(CenterButton)
		|| input_->PressMouse(RightButton)) {
		pos.y += 2.0f;
	}
	sprite2_->SetPosition(pos);
	sprite2_->SetColor(Vector4(0.8f, 0.2f, 0.5f, 0.9f));
	sprite2_->SetSize(Vector2(140, 50));
	sprite2_->SetIsFlipY(true);
	sprite2_->Update();

	sprite3_->SetPosition(input_->GetMousePosition());
	sprite3_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	sprite3_->SetSize({ 100,100 });
	sprite3_->SetTextureSize({256,256});
	sprite3_->Update();
}

void GamePlayScene::Draw(){
	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	skydome_->Draw();
	ground_->Draw();

	object3d_2->Draw();
	object3d_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	sprite_->Draw();
	sprite2_->Draw();
	sprite3_->Draw();

	drawBas_->PostDraw();
}

void GamePlayScene::Finalize(){
	SafeDelete(skydome_);
	SafeDelete(ground_);
	SafeDelete(object3d_);
	SafeDelete(object3d_2);
	SafeDelete(model_);
	SafeDelete(model_2);
	SafeDelete(modelSkydome_);
	SafeDelete(modelGround_);

	SafeDelete(sprite_);
	SafeDelete(sprite2_);
	SafeDelete(sprite3_);
}