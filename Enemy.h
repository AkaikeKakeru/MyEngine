#pragma once
#include "Input.h"
#include "Object3d.h"
#include "WorldTransform.h"

class Enemy {
public:
	void Initialize(Model* model,Vector3 position);
	void Update();
	void Draw();
	void Finalize();

	//衝突コールバック
	void OnCollision();

	const Vector3& GetPosition()const { return object_->GetPosition(); }
	float GetRadian()const { return radian_; }

public:
	bool IsDead() const { return isDead_; }

private:
	//寿命
	static const int32_t kLifeTime = 60 * 3;

private:
	static Input* input_;
	Object3d* object_ = nullptr;
	WorldTransform worldTransform_;

	float speed_ = 2.4f;

	float radian_ = 5.8f;

	//デスタイマー
	int32_t deathTimer_ = kLifeTime;
	//デスフラグ
	bool isDead_ = false;

public:
	~Enemy();
};