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
	//オブジェクト移動
	if (input_->PressKey(DIK_UP) || input_->PressKey(DIK_DOWN) || input_->PressKey(DIK_RIGHT) || input_->PressKey(DIK_LEFT)) {
		// 現在の座標を取得
		Vector3 position = object_->GetPosition();

		// 移動後の座標を計算
		if (input_->PressKey(DIK_UP)) { position.y += 1.0f; }
		else if (input_->PressKey(DIK_DOWN)) { position.y -= 1.0f; }
		if (input_->PressKey(DIK_RIGHT)) { position.x += 1.0f; }
		else if (input_->PressKey(DIK_LEFT)) { position.x -= 1.0f; }

		// 座標の変更を反映
		object_->SetPosition(position);
	}

	//更新
	object_->Update();
}

void Player::Draw() {
	object_->Draw();
}

Player::~Player() {
	delete object_;
}