#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Collision.h"
#include <random>

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();

void GamePlayScene::Initialize(){
	Initialize3d();

	Initialize2d();
}

void GamePlayScene::Update(){
	input_->Update();

	Update3d();

	Update2d();
}

void GamePlayScene::Draw(){
	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	Draw3d();
	Object3d::PostDraw();

	drawBas_->PreDraw();
	Draw2d();
	drawBas_->PostDraw();
}

void GamePlayScene::Initialize3d(){
	//天球
	modelSkydome_ = Model::LoadFromOBJ("skydome");
	std::unique_ptr<Skydome> newSkydome = std::make_unique<Skydome>();
	newSkydome->Initialize(modelSkydome_);
	skydome_.push_back(std::move(newSkydome));

	//自機
	modelPlayer_ = Model::LoadFromOBJ("plane");
	std::unique_ptr<Player> newPlayer = std::make_unique<Player>();
	newPlayer->Initialize(modelPlayer_);
	player_.push_back(std::move(newPlayer));

	//敵機
	modelEnemy_ = Model::LoadFromOBJ("planeEnemy");
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Initialize(modelEnemy_,{
		RandomOutput(-62.0f,62.0f),
		RandomOutput(-38.0f,38.0f),
		100.0f});
	enemys_.push_back(std::move(newEnemy));
}

void GamePlayScene::Initialize2d(){
	//描画基盤
	drawBas_->LoadTexture(0, "smile.png");
	drawBas_->LoadTexture(1, "texture.png");

	//描画スプライト
	//レティクル
	reticle_ = new Sprite();
	reticle_->Initialize(drawBas_,1);
}

void GamePlayScene::Update3d(){
	//デスフラグが立ったら削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});

	//レベルアップタイマーカウントダウン
	levelUpTimer_--;

	if (levelUpTimer_ <= 0) {
		spawnNum_++;
		levelUpTimer_ = kLevelInterval;
	}

	//湧きタイマーカウントダウン
	spawnTimer_--;

	if (spawnTimer_ <= 0) {

		for (size_t i = 0; i < spawnNum_; i++){
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Initialize(modelEnemy_,{
				RandomOutput(-62.0f,62.0f),
				RandomOutput(-38.0f,38.0f),
				100.0f});
			enemys_.push_back(std::move(newEnemy));
		}

		spawnTimer_ = kSpawnInterval;
	}

	Vector2 pos = { 0,0 };

	// カメラ移動
	if (input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_A) ||
		input_->PressKey(DIK_Q) ||
		input_->PressKey(DIK_C)) {

		Vector3 eyeVector = { 0,0,0 };

		if (input_->PressKey(DIK_W)) { eyeVector.y += 1.0f; }
		else if (input_->PressKey(DIK_S)) { eyeVector.y -= 1.0f; }
		if (input_->PressKey(DIK_D)) { eyeVector.x += 1.0f; }
		else if (input_->PressKey(DIK_A)) { eyeVector.x -= 1.0f; }
		if (input_->PressKey(DIK_Q)) { eyeVector.z  += 1.0f; }
		else if (input_->PressKey(DIK_C)) { eyeVector.z -= 1.0f;}
	
		Object3d::CameraMoveEyeVector(eyeVector);
	}

	for (std::unique_ptr<Skydome>& skydome:skydome_){
		skydome->Update();
	}

	for (std::unique_ptr<Player>& player:player_){
		player->Update();
	}

	for (std::unique_ptr<Enemy>& enemy:enemys_){
		enemy->Update();
	}

	CheckAllCollisions();
}

void GamePlayScene::Update2d(){
	reticle_->SetPosition(input_->GetMousePosition());
	reticle_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	reticle_->SetSize({ 100,100 });
	reticle_->SetTextureSize({256,256});
	reticle_->Update();
}

void GamePlayScene::Draw3d(){
	for (std::unique_ptr<Skydome>& skydome:skydome_){
		skydome->Draw();
	}

	for (std::unique_ptr<Enemy>& enemy:enemys_){
		enemy->Draw();
	}

	for (std::unique_ptr<Player>& player:player_){
		player->Draw();
	}
}

void GamePlayScene::Draw2d(){
	reticle_->Draw();
}

void GamePlayScene::Finalize(){
	for (std::unique_ptr<Enemy>& enemy:enemys_){
		enemy->Finalize();
	}

	//デスフラグが立ったら削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});

	player_.remove_if([](std::unique_ptr<Player>& player) {
		return true;
		});

	skydome_.remove_if([](std::unique_ptr<Skydome>& skydome) {
		return true;
		});
	
	SafeDelete(modelSkydome_);
	SafeDelete(modelEnemy_);
	SafeDelete(modelPlayer_);

	SafeDelete(reticle_);
}

void GamePlayScene::CheckAllCollisions() {
	Vector3 posA, posB;
	float radA, radB;

	for (const std::unique_ptr<Player>& player : player_) {
		posA = player->GetPosition();
		radA = player->GetRadian();

		for (const std::unique_ptr<Enemy>& enemy : enemys_) {
			posB = enemy->GetPosition();
			radB = enemy->GetRadian();

			if (Collision_SphereToSphere(posA, posB, radA, radB)) {
				player->OnCollision();
				enemy->OnCollision();
			}
		}
	}
}

float GamePlayScene::RandomOutput(float min, float max){
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> dist(min,max);

	return dist(engine);
}