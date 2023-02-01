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
	//カメラ生成
	camera_ = new Camera();
	Object3d::SetCamera(camera_);

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome",false);


	planeObj_ = new Object3d();
	planeObj_ = Object3d::Create();
	planeObj_->SetModel(planeModel_);
	planeObj_->SetScale({ 1, 1, 1 });

	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetScale({ 30, 30, 30 });

	//ライト生成
	light_ = new Light();
	light_ = Light::Create();
	light_->SetLightColor({ 1,1,1 });
	Object3d::SetLight(light_);
}

void GamePlayScene::Initialize2d() {
	drawBas_->LoadTexture(1, "texture.png");
	sprite_->Initialize(drawBas_, 1);

	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->SetSize({ 64,64 });

}

void GamePlayScene::Update3d() {
	skydomeObj_->Update();
	planeObj_->Update();

	// オブジェクト移動
	if (input_->PressKey(DIK_UP) ||
		input_->PressKey(DIK_DOWN) || 
		input_->PressKey(DIK_RIGHT) || 
		input_->PressKey(DIK_LEFT))
	{
		// 現在の座標を取得
		Vector3 position = planeObj_->GetPosition();

		//移動後の座標を計算
		if (input_->PressKey(DIK_UP)) { position.y += 0.5f; }
		else if (input_->PressKey(DIK_DOWN)) { position.y -= 0.5f; }
		if (input_->PressKey(DIK_RIGHT)) { position.x += 0.5f; }
		else if (input_->PressKey(DIK_LEFT)) { position.x -= 0.5f; }

		// 座標の変更を反映
		planeObj_->SetPosition(position);
	}

	// オブジェクト回転
	{
		// 現在の座標を取得
		Vector3 rot = planeObj_->GetRotation();

		// 移動後の座標を計算
		rot.y += ConvertToRadian(1.0f);

		// 座標の変更を反映
		planeObj_->SetRotation(rot);
	}

	{
		static Vector3 lightDir = { 0,1,5 };

		if (input_->PressKey(DIK_W) ||
			input_->PressKey(DIK_S) ||
			input_->PressKey(DIK_D) ||
			input_->PressKey(DIK_A)) {
			if (input_->PressKey(DIK_W)) { lightDir.y += 1.0f; }
			else if (input_->PressKey(DIK_S)) {lightDir.y -= 1.0f; }
			if (input_->PressKey(DIK_D)) { lightDir.x += 1.0f; }
			else if (input_->PressKey(DIK_A)) { lightDir.x -= 1.0f; }
		}

		light_->SetLightDir(lightDir);

	}

	light_->Update();
	planeObj_->Update();
}

void GamePlayScene::Update2d() {
	// 現在の座標を取得
	Vector2 position = input_->GetMousePosition();

	//移動後の座標を計算
	
	// 座標の変更を反映
	sprite_->SetPosition(position);

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

	SafeDelete(light_);
	SafeDelete(camera_);
}
