#include "SceneManager.h"

void SceneManager::Update(){
	//次のシーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		//シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		//シーンマネージャをセット
		//scene_->SetSceneManager(this);

		//次のシーンを初期化
		scene_->Initialize();
	}

	//実行中シーンの更新
	scene_->Update();
}

void SceneManager::Draw(){
	scene_->Draw();
}

SceneManager::~SceneManager(){
	//最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
}