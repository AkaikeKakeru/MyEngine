#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Collision.h"
#include <random>

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();

void GamePlayScene::Initialize() {
	//湧きタイマー
	spawnTimer_ = kSpawnInterval;
	//湧き数
	spawnNum_ = 1;

	//レベルアップタイマー
	levelUpTimer_ = kLevelInterval;

	//ヒットストップフラグ
	isHitStop_ = false;
	//ヒットストップタイマー
	hitStopTimer_ = kHitStopInterval;

	//無敵フラグ
	isInvincible_ = false;
	//無敵タイマー
	invincibleTimer_ = kInvincibleInterval;

	//ゲームオーバーフラグ
	isGameOver_ = false;

	Initialize3d();

	Initialize2d();
}

void GamePlayScene::Update() {
	input_->Update();

	//自機の衝突判定時に、ヒットストップを起動
	if (player_->IsCollision() && !isInvincible_) {
		isHitStop_ = true;
		isInvincible_ = true;
	}

	if (isHitStop_ == true) {
		//ヒットストップタイマーカウントダウン
		hitStopTimer_--;

		if (hitStopTimer_ <= 0) {
			hitStopTimer_ = kHitStopInterval;
			isHitStop_ = false;
		}

	}
	else {
		if (!isGameOver_) {
			Update3d();
		}
	}
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
	//天球
	modelSkydome_ = Model::LoadFromOBJ("skydome");
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	//自機
	modelPlayer_ = Model::LoadFromOBJ("plane");
	player_ = new Player();
	player_->Initialize(modelPlayer_);

	//敵機
	modelEnemy_ = Model::LoadFromOBJ("planeEnemy");
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Initialize(modelEnemy_, { 0.0f,30.0f,100.0f });
	enemys_.push_back(std::move(newEnemy));
}

void GamePlayScene::Initialize2d() {
	//描画基盤
	//描画スプライト
	back_ = new Sprite();
	ui_ = new Sprite();
	over_ = new Sprite();

	over_->Initialize(drawBas_, 3);
	over_->SetAnchorPoint({ 0.5f,0.5f });
	over_->SetPosition({ WinApp::Win_Width / 2 + 100.0f,WinApp::Win_Height / 2 - 150.0f });
	over_->Update();

	back_->Initialize(drawBas_, 2);
	back_->SetAnchorPoint({ 0.5f,0.5f });
	back_->SetPosition({ WinApp::Win_Width / 2,WinApp::Win_Height / 2 });
	back_->Update();

	ui_->Initialize(drawBas_, 1);
	ui_->SetAnchorPoint({ 0.5f,0.5f });
	ui_->SetPosition({ WinApp::Win_Width / 2 + 260.0f,WinApp::Win_Height / 2 + 160.0f });
	ui_->Update();
}

void GamePlayScene::Update3d() {
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

		for (size_t i = 0; i < spawnNum_; i++) {
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Initialize(modelEnemy_, {
				RandomOutput(-62.0f,62.0f),
				RandomOutput(-38.0f,38.0f),
				100.0f });
			enemys_.push_back(std::move(newEnemy));
		}

		spawnTimer_ = kSpawnInterval;
	}

	skydome_->Update();

	player_->Update();

	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Update();
	}

	if (isInvincible_ == true) {
		//無敵タイマーカウントダウン
		invincibleTimer_--;

		if (invincibleTimer_ <= 0) {
			invincibleTimer_ = kInvincibleInterval;
			isInvincible_ = false;
		}
	}
	else {
		CheckAllCollisions();
	}

	if (player_->IsGameOver()) {
		isGameOver_ = true;
	}
}

void GamePlayScene::Update2d() {
}

void GamePlayScene::Draw3d() {
	skydome_->Draw();

	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw();
	}

	if (invincibleTimer_ % 2 == 0) {
		player_->Draw();
	}
}

void GamePlayScene::Draw2d() {
	if (isGameOver_) {
		back_->Draw();
		ui_->Draw();
		over_->Draw();
	}
}

void GamePlayScene::Finalize() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Finalize();
	}

	//デスフラグが立ったら削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});

	SafeDelete(ui_);
	SafeDelete(back_);
	SafeDelete(over_);

	SafeDelete(skydome_);
	SafeDelete(player_);

	SafeDelete(modelSkydome_);
	SafeDelete(modelEnemy_);
	SafeDelete(modelPlayer_);
}

void GamePlayScene::CheckAllCollisions() {
	Vector3 posA, posB;
	float radA, radB;

	posA = player_->GetPosition();
	radA = player_->GetRadian();

	for (const std::unique_ptr<Enemy>& enemy : enemys_) {
		posB = enemy->GetPosition();
		radB = enemy->GetRadian();

		if (Collision_SphereToSphere(posA, posB, radA, radB)) {
			player_->OnCollision();
			enemy->OnCollision();
		}
	}
}

float GamePlayScene::RandomOutput(float min, float max) {
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(engine);
}