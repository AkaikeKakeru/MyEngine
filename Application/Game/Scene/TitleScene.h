#pragma once
#include "BaseScene.h"

#include "Input.h"
#include "DrawBasis.h"
#include "ObjectModel.h"
#include "Sprite.h"
#include "Object3d.h"
#include "FbxObject3d.h"

#include "ParticleManager.h"

#include "Camera.h"
#include "LightGroup.h"

#include "SceneManager.h"

class TitleScene : public BaseScene{
public://構造体

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	static DirectXBasis* dxBas_;
	static Input* input_;
	DrawBasis* drawBas_ = nullptr;

	Camera* camera_ = nullptr;
	LightGroup* light_ = nullptr;
	/// <summary>
	/// オブジェクト
	/// </summary>
	/// <summary>
	Object3d* planeObj_ = nullptr;
	ObjectModel* planeModel_ = nullptr;

	Object3d* skydomeObj_ = nullptr;
	ObjectModel* skydomeModel_ = nullptr;
	/// スプライト
	/// </summary>
	Sprite* sprite_ = new Sprite();

	//FBX
	FbxModel* model1 = nullptr;
	FbxObject3d* object1 = nullptr;

	//FBX
	FbxModel* model2 = nullptr;
	FbxObject3d* object2 = nullptr;

	//パーティクル
	ParticleManager* particleManager_ = nullptr;
	int particleNum_ = 0;
};