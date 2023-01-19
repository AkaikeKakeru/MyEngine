#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "Player.h"
#include "Skydome.h"

class TitleScene : public BaseScene {

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	/// <summary>
	/// オブジェクト
	/// </summary>
	/// <summary>
	/// スプライト
	/// </summary>
	static DirectXBasis* dxBas_;
	static Input* input_;
	DrawBasis* drawBas_ = nullptr;

	Model* modelSkydome_ = nullptr;
	Skydome* skydome_ = nullptr;

	Model* modelPlayer_ = nullptr;
	//Player* player_ = nullptr;

	Sprite* title_ = nullptr;
	Sprite* back_ = nullptr;
	Sprite* ui_ = nullptr;

	bool isStart_ = false;

	Vector3 titleStartPos_ = {};
	Vector3 titleEndPos_ = {};
	Vector3 backStartPos_ = {};
	Vector3 backEndPos_ = {};
	Vector3 uiStartPos_ = {};
	Vector3 uiEndPos_ = {};

	float MaxTime_ = 2.0f;
	float timeRate_;

	float count_ = 0.0f;
};