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
#include "GamePlayScene.h"
#include "Framework.h"

class GameMain :public FrameworkΓ{
public://構造体
	//マウスボタン
	typedef enum MouseButtonNum {
		LeftButton,
		RightButton,
		CenterButton,
	}MouseButtonNum;

public://関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private://変数
	BaseScene* scene_ = nullptr;
};