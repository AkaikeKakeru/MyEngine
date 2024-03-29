#pragma once
#include "Input.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "Sprite.h"

class Player {
public:
	void Initialize(Sprite*sprite,Model* model);
	void Update();
	void Draw();
	void Draw2d();

	//衝突コールバック
	void OnCollision();

	const Vector3& GetPosition()const { return object_->GetPosition(); }

	float GetRadian()const { return radian_; }
	bool IsCollision()const { return isCollision_; }

	bool IsGameOver()const { return isGameOver_; }
private:
	void Move();

	void Rotation();

	void Reticle();

private:
	static Input* input_;
	Object3d* object_ = nullptr;
	WorldTransform worldTransform_;

	Object3d* objectReticle_ = nullptr;
	WorldTransform worldTransformReticle_;

	Sprite* spriteRethicle_ = nullptr;

	float speed_ = 1.0f;
	float speed_slow = 0.7f;

	float gravity_ = 0.01f;

	float radian_ = 5.8f;

	bool isCollision_ = false;

	bool isGameOver_ = false;
public:
	~Player();
};