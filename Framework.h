#pragma once
#include "WinApp.h"
#include "DirectXBasis.h"
#include "Input.h"

#include "SceneManager.h"

class FrameworkΓ {
public:
	//実行
	void Run();
	virtual void Initialize();
	virtual void Update();
	virtual void Draw() = 0;
	virtual void Finalize();

	virtual bool IsEndRequest() { return isEndRequest_; }

	//SceneManager* GetSceneManager()const { return sceneManager_; }

	WinApp* GetWinApp()const { return winApp_; }
	DirectXBasis* GetDirectXBasis()const { return dxBas_; }
	Input* GetInput()const { return input_; }

private:
	WinApp* winApp_ = nullptr;
	DirectXBasis* dxBas_ = nullptr;
	Input* input_ = nullptr;

	bool isEndRequest_ = false;

	//シーン
	//SceneManager* sceneManager_ = nullptr;

public: 
	virtual ~FrameworkΓ() = default;
};