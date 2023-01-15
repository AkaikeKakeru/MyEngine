#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"

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

	//衝突確認
	void CheckAllCollisions();
private:
	void Initialize3d();
	void Initialize2d();

	void Update3d();
	void Update2d();

	void Draw3d();
	void Draw2d();

	float RandomOutput(float min,float max);

public:
	//生成間隔
	static const int kSpawnInterval = 60 * 1;

	static const int kLevelInterval = 60 * 4;

private:
	/// <summary>
	/// オブジェクト
	/// </summary>
	Model* modelSkydome_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	static DirectXBasis* dxBas_;
	static Input* input_;

	static DrawBasis* drawBas_;

	Sprite* reticle_ = nullptr;

	std::list<std::unique_ptr<Player>> player_;
	std::list<std::unique_ptr<Enemy>> enemys_;
	std::list<std::unique_ptr<Skydome>> skydome_;

	//湧きタイマー
	int32_t spawnTimer_ = kSpawnInterval;
	//湧き数
	int32_t spawnNum_ = 1;

	//レベルアップタイマー
	int32_t levelUpTimer_ = kLevelInterval;
};