#pragma once

//前方宣言
class SceneManager;

class BaseScene {
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}

	//virtual SceneManager* GetSceneManager()const {
	//	return sceneManager_; 
	//}

private:
	//シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;

public:
	virtual ~BaseScene() = default;
};