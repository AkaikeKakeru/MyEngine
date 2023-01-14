#include "GamePlayScene.h"
#include "SafeDelete.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();

void GamePlayScene::Initialize(){
	/// 描画初期化

	//オブジェクトモデル
	modelSkydome_ = Model::LoadFromOBJ("skydome");
	skydome_ = Object3d::Create();
	skydome_->SetModel(modelSkydome_);

	modelPlayer_ = Model::LoadFromOBJ("plane");
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_);

	modelEnemy_ = Model::LoadFromOBJ("planeEnemy");
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Initialize(modelEnemy_);
	enemys_.push_back(std::move(newEnemy));

	//描画基盤
	drawBas_->LoadTexture(0, "smile.png");
	drawBas_->LoadTexture(1, "texture.png");

	//描画スプライト
	reticle_ = new Sprite();
	reticle_->Initialize(drawBas_,1);
}

void GamePlayScene::Update(){
	input_->Update();

	if (input_->TriggerKey(DIK_RETURN)) {

		std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(modelEnemy_);
		enemys_.push_back(std::move(newEnemy));
	}

	Vector2 pos = { 0,0 };

	//オブジェクト更新

	// カメラ移動
	if (input_->PressKey(DIK_W) || input_->PressKey(DIK_S) || input_->PressKey(DIK_D) || input_->PressKey(DIK_A)) {
		if (input_->PressKey(DIK_W)) { Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f }); }
		else if (input_->PressKey(DIK_S)) { Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f }); }
		if (input_->PressKey(DIK_D)) { Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f }); }
		else if (input_->PressKey(DIK_A)) { Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f }); }
	}

	skydome_->Update();

	player_->Update();

	for (std::unique_ptr<Enemy>& enemy:enemys_){
		enemy->Update();
	}

	reticle_->SetPosition(input_->GetMousePosition());
	reticle_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	reticle_->SetSize({ 100,100 });
	reticle_->SetTextureSize({256,256});
	reticle_->Update();
}

void GamePlayScene::Draw(){
	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	skydome_->Draw();

	for (std::unique_ptr<Enemy>& enemy:enemys_){
		enemy->Draw();
	}

	player_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	reticle_->Draw();

	drawBas_->PostDraw();
}

void GamePlayScene::Finalize(){
	SafeDelete(skydome_);
	SafeDelete(modelSkydome_);

	SafeDelete(plane_);
	SafeDelete(planeEnemy_);
	SafeDelete(modelEnemy_);
	SafeDelete(modelPlayer_);
	
	//SafeDelete(player_);
	//SafeDelete(enemy_);

	SafeDelete(reticle_);
}