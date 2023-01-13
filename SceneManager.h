#pragma once
#include "BaseScene.h"

class SceneManager {
public:
	void Update();
	void Draw();

	//次シーン予約
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }
private:
	//現在シーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;
public:
	~SceneManager();
};