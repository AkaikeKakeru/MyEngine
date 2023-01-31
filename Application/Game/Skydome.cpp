#include "Skydome.h"
#include "Vector2.h"
#include "Vector3.h"

void Skydome::Initialize(Model* model) {
	object_ = Object3d::Create();
	object_->SetModel(model);
}

void Skydome::Update() {
	//オブジェクト移動

	//更新
	object_->Update();
}

void Skydome::Draw() {
	object_->Draw();
}

Skydome::~Skydome() {
	delete object_;
}