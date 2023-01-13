#pragma once
#include <Windows.h>
#include "SafeDelete.h"
#include "WinApp.h"
#include "DirectXBasis.h"
#include "Input.h"
#include "ObjectBasis.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Degree.h"
#include "Object3d.h"

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

	/// <summary>
	/// オブジェクト
	/// </summary>

	Model* model_ = nullptr;
	Model* model_2 = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelGround_ = nullptr;

	Object3d* object3d_ = nullptr;
	Object3d* object3d_2 = nullptr;
	Object3d* skydome_ = nullptr;
	Object3d* ground_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>

	DrawBasis* drawBas_ = nullptr;

	Sprite* sprite_ = new Sprite();
	Sprite* sprite2_ = new Sprite();
	Sprite* sprite3_ = new Sprite();
};