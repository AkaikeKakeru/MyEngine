#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "TitleScene.h"
#include "SceneManager.h"

class GamePlayScene : public BaseScene{
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

	Model* model_ = nullptr;
	Model* model_2 = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelGround_ = nullptr;

	Model* modelPlane_ = nullptr;

	Object3d* object3d_ = nullptr;
	Object3d* object3d_2 = nullptr;
	Object3d* skydome_ = nullptr;
	Object3d* ground_ = nullptr;

	Object3d* plane_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	static DirectXBasis* dxBas_;
	static Input* input_;

	DrawBasis* drawBas_ = nullptr;

	Sprite* sprite_ = new Sprite();
	Sprite* sprite2_ = new Sprite();
	Sprite* sprite3_ = new Sprite();

	//シーン
	SceneManager* sceneManager_ = nullptr;
};