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

public://関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:

private://変数
	static Input* input_;
	BaseScene* scene_ = nullptr;
};