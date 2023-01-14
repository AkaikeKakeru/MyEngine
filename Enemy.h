#pragma once
#include "Input.h"
#include "Object3d.h"
#include "WorldTransform.h"

class Enemy {
public:
	void Initialize(Model* model);
	void Update();
	void Draw();
	void Finalize();

public:
	bool IsDead() const { return isDead_; }

private:
	//寿命
	static const int32_t kLifeTime = 60 * 5;

private:
	static Input* input_;
	Object3d* object_ = nullptr;
	WorldTransform worldTransform_;

	//デスタイマー
	int32_t deathTimer_ = kLifeTime;
	//デスフラグ
	bool isDead_ = false;

public:
	~Enemy();
};