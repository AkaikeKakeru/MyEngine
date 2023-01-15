#pragma once
#include <Windows.h>
#include "SafeDelete.h"
#include "WinApp.h"
#include "DirectXBasis.h"
#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Degree.h"
#include "Framework.h"
#include "BaseScene.h"

class GameMain :public FrameworkΓ {
public://構造体

	enum SceneNum {
		Title_scene,
		Wait_scene,
		GamePlay_scene
	};

public://関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	//開始間隔
	static const int kStartInterval = 60 * 1;

private://変数
	static Input* input_;
	BaseScene* scene_ = nullptr;

	int32_t sceneNum = Title_scene;

	//開始タイマー
	int32_t startTimer_ = kStartInterval;
};