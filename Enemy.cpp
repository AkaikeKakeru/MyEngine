#include "Enemy.h"
#include "Vector2.h"
#include "Vector3.h"

Input* Enemy::input_ = Input::GetInstance();

void Enemy::Initialize(Model* model) {
	object_ = Object3d::Create();

	object_->SetModel(model);
	object_->SetScale({ 2.0f,2.0f,2.0f });
	object_->SetPosition({ 0.0f,0.0f,50.0f });
}

void Enemy::Update() {
	//時間経過で消滅
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	//オブジェクト移動
	// 現在の座標を取得
	Vector3 position = object_->GetPosition();

	// 移動後の座標を計算
	position.z -= 0.3f;

	// 座標の変更を反映
	object_->SetPosition(position);

	//更新
	object_->Update();
}

void Enemy::Draw() {
	object_->Draw();
}

Enemy::~Enemy() {
	delete object_;
}