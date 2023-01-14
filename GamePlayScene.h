#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

#include "TitleScene.h"
#include "SceneManager.h"

#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"

#include <memory>
#include <List>

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
	void Update3d();
	void Update2d();

	//static const int kMaxEnemyNum = 64;

private:
	/// <summary>
	/// オブジェクト
	/// </summary>
	Model* modelSkydome_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;

	//Object3d* skydome_ = nullptr;
	//Object3d* plane_ = nullptr;
	//Object3d* planeEnemy_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	static DirectXBasis* dxBas_;
	static Input* input_;

	static DrawBasis* drawBas_;

	Sprite* reticle_ = nullptr;

	std::unique_ptr<Player> player_;
	std::list<std::unique_ptr<Enemy>> enemys_;
	std::unique_ptr<Skydome> skydome_;

	//シーン
	SceneManager* sceneManager_ = nullptr;
};