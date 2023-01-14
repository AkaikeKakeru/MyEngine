#pragma once
#include "Input.h"
#include "Object3d.h"
#include "WorldTransform.h"

class Player {
public:
	void Initialize(Model* model);
	void Update();
	void Draw();

	//衝突コールバック
	void OnCollision();

	const Vector3& GetPosition()const { return object_->GetPosition(); }

	float GetRadian()const { return radian_; }

private:
	static Input* input_;
	Object3d* object_ = nullptr;
	WorldTransform worldTransform_;

	float gravity_ = 0.01f;

	float radian_ = 5.8f;
public:
	~Player();
};