#pragma once
#include "Input.h"
#include "Object3d.h"
#include "WorldTransform.h"

class Player {
public:
	void Initialize(Model* model);
	void Update();
	void Draw();
private:
	static Input* input_;
	Object3d* object_ = nullptr;
	WorldTransform worldTransform_;
public:
	~Player();
};