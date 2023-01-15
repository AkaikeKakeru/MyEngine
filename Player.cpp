#include "Player.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Degree.h"

Input* Player::input_ = Input::GetInstance();

void Player::Initialize(Model* model) {

	speed_ = 1.0f;
	speed_slow = 0.7f;

	gravity_ = 0.01f;

	radian_ = 5.8f;

	isCollision_ = false;

	isGameOver_ = false;

	object_ = Object3d::Create();

	object_->SetModel(model);
	object_->SetScale({ 2.0f,2.0f,2.0f });
	object_->SetRotation({ 0,ConvertToRadian(180),0 });
}

void Player::Update() {
	if (isCollision_) {
		isCollision_ = false;
	}

	Move();
	Rotation();

	//更新
	object_->Update();

	if (object_->GetPosition().y <= -42.0f) {
		isGameOver_ = true;
	}
}

void Player::Draw() {
	object_->Draw();
}

void Player::OnCollision() {
	gravity_ += 0.07f;
	isCollision_ = true;
}

void Player::Move() {
	// 現在の座標を取得
	Vector3 position = {};

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

	position.y -= gravity_;

	// 座標の変更を反映
	object_->SetPosition(object_->GetPosition() + position);
}

void Player::Rotation() {
	Vector3 rotation = {};
	if (input_->PressKey(DIK_UP)) {
		if (object_->GetRotation().x < ConvertToRadian(15.0f)) {
			rotation.x += ConvertToRadian(1.0f);
		}
	}
	else {
		if (object_->GetRotation().x > 0.0) {
			rotation.x -= ConvertToRadian(1.0f);
		}
	}

	if (input_->PressKey(DIK_DOWN)) {
		if (object_->GetRotation().x > ConvertToRadian(-15.0f)) {
			rotation.x -= ConvertToRadian(1.0f);
		}
	}
	else {
		if (object_->GetRotation().x < 0.0) {
			rotation.x += ConvertToRadian(1.0f);
		}
	}

	if (input_->PressKey(DIK_RIGHT)) {
		if (object_->GetRotation().z < ConvertToRadian(15.0f)) {
			rotation.z += ConvertToRadian(1.0f);
		}
	}
	else {
		if (object_->GetRotation().z > 0.0) {
			rotation.z -= ConvertToRadian(1.0f);
		}
	}

	if (input_->PressKey(DIK_LEFT)) {
		if (object_->GetRotation().z > ConvertToRadian(-15.0f)) {
			rotation.z -= ConvertToRadian(1.0f);
		}
	}
	else {
		if (object_->GetRotation().z < 0.0) {
			rotation.z += ConvertToRadian(1.0f);
		}
	}


	// 座標の変更を反映
	object_->SetRotation(object_->GetRotation() + rotation);
}

Player::~Player() {
	delete object_;
}