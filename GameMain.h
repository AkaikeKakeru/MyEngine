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
#include "TitleScene.h"
#include "GamePlayScene.h"

class GameMain {
public://構造体
	//マウスボタン
	typedef enum MouseButtonNum {
		LeftButton,
		RightButton,
		CenterButton,
	}MouseButtonNum;

public://関数
	void Run();
	void Initialize();
	void Update();
	void Draw();
	void Finalize();
	bool IsEndRequest() { return isEndRequest_; };
private://変数
	WinApp* winApp_ = nullptr;
	DirectXBasis* dxBas_ = nullptr;
	Input* input_ = nullptr;

	bool isEndRequest_ = false;

	//シーン
	TitleScene* scene_ = nullptr;
	//GamePlayScene* scene_ = nullptr;
};