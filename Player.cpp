#include "Player.h"
#include "Vector2.h"
#include "Vector3.h"

Input* Player::input_ = Input::GetInstance();

void Player::Initialize(Model* model) {
	object_ = Object3d::Create();

	object_->SetModel(model);
	object_->SetScale({ 2.0f,2.0f,2.0f });
	object_->SetRotation({ 0,ConvertToRadian(180),0 });
}

void Player::Update() {
	// 現在の座標を取得
	Vector3 position = object_->GetPosition();

	//オブジェクト移動
	// 移動後の座標を計算
	if (input_->PressKey(DIK_UP)) {
		if (input_->PressKey(DIK_RIGHT) ||
			input_->PressKey(DIK_LEFT)) {
			if (input_->PressKey(DIK_RIGHT)) {
				position.x += speed_slow;
			}
			if (input_->PressKey(DIK_LEFT)) {
				position.x -= speed_slow;
			}
			position.y += speed_slow;
		}
		else {
			position.y += speed_;
		}
	}

	else if (input_->PressKey(DIK_DOWN)) {
		if (input_->PressKey(DIK_RIGHT) ||
			input_->PressKey(DIK_LEFT)) {
			if (input_->PressKey(DIK_RIGHT)) {
				position.x += speed_slow;
			}
			if (input_->PressKey(DIK_LEFT)) {
				position.x -= speed_slow;
			}
			position.y -= speed_slow;
		}
		else {
			position.y -= speed_;
		}
	}

	else if (input_->PressKey(DIK_RIGHT)) {
		position.x += speed_;
	}

	else if (input_->PressKey(DIK_LEFT)) {
		position.x -= speed_;
	}

	if (position.x > 62.0f) {
		position.x = 62.0f;
	}

	if (position.x < -62.0f) {
		position.x = -62.0f;
	}

	if (position.y > 38.0f) {
		position.y = 37.5f;
	}

	if (position.y < -38.0f) {
		position.y = -37.5f;
	}

	position.y -= gravity_;

	// 座標の変更を反映
	object_->SetPosition(position);

	//更新
	object_->Update();
}

void Player::Draw() {
	object_->Draw();
}

void Player::OnCollision() {
	gravity_ += 0.07f;
}

Player::~Player() {
	delete object_;
}