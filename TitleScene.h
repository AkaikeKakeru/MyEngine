#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

class TitleScene : public BaseScene{
public://構造体
	   //マウスボタン
	typedef enum MouseButtonNum {
		LeftButton,
		RightButton,
		CenterButton,
	}MouseButtonNum;

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

	Sprite* sprite_ = new Sprite();

	//シーン
	//SceneManager* sceneManager_;
};