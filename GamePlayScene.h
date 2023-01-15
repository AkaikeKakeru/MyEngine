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

class GamePlayScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	//衝突確認
	void CheckAllCollisions();

	bool IsGameOver() { return isGameOver_; };
private:
	void Initialize3d();
	void Initialize2d();

	void Update3d();
	void Update2d();

	void Draw3d();
	void Draw2d();

	float RandomOutput(float min, float max);

public:
	//生成間隔
	static const int kSpawnInterval = 60 * 1;

	//レベル間隔
	static const int kLevelInterval = 60 * 4;

	//ヒットストップ間隔
	static const int kHitStopInterval = 24;//60*0.4

	//無敵間隔
	static const int kInvincibleInterval = 78;//60*1.3

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

	Sprite* back_ = nullptr;
	Sprite* ui_ = nullptr;
	Sprite* over_ = nullptr;

	Player* player_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemys_;

	Skydome* skydome_ = nullptr;

	//湧きタイマー
	int32_t spawnTimer_ = kSpawnInterval;
	//湧き数
	int32_t spawnNum_ = 1;

	//レベルアップタイマー
	int32_t levelUpTimer_ = kLevelInterval;

	//ヒットストップフラグ
	bool isHitStop_ = false;
	//ヒットストップタイマー
	int32_t hitStopTimer_ = kHitStopInterval;

	//無敵フラグ
	bool isInvincible_ = false;
	//無敵タイマー
	int32_t invincibleTimer_ = kInvincibleInterval;

	//ゲームオーバーフラグ
	bool isGameOver_ = false;
};