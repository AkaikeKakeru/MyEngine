#pragma once
#include "Object3d.h"
#include "WorldTransform.h"

class Skydome {
public:
	void Initialize(Model* model);
	void Update();
	void Draw();
private:
	Object3d* object_ = nullptr;
	WorldTransform worldTransform_;
public:
	~Skydome();
};