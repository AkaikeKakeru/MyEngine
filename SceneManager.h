#pragma once
#include "BaseScene.h"

class SceneManager final{
public:
	void Update();
	void Draw();
	void Finalize();

	//次シーン予約
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }

private:
	//現在シーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;

private:
	SceneManager() = default;
	~SceneManager() = default;
	//コピーコンストラクタを無効
	SceneManager(const SceneManager&) = delete;
	//代入演算子を無効
	const SceneManager& operator=(const SceneManager&) = delete;

public://static
	static SceneManager* GetInstance();
};