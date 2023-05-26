#include "TitleScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();

void TitleScene::Initialize(){
	/// 描画初期化
	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get());

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();
	camera_->SetEye({ 0,20.0f,-100.0f });
	camera_->SetTarget({ 0,20.0f,0 });

	//FBX
	//デバイスセット
	FbxObject3d::SetDevice(DirectXBasis::GetInstance()->GetDevice().Get());
	//カメラセット
	FbxObject3d::SetCamera(camera_);

	FbxObject3d::CreateGraphicsPipeline();

	model1 = FbxLoader::GetInstance()->LoadModelFromFile("cube");

	object1 = new FbxObject3d();
	object1->Initialize();
	object1->SetModel(model1);

	//各種OBJ
	planeModel_ = new ObjectModel();
	planeModel_ = ObjectModel::LoadFromOBJ("plane", false);

	skydomeModel_ = new ObjectModel();
	skydomeModel_ = ObjectModel::LoadFromOBJ("skydome",false);


	planeObj_ = new Object3d();
	planeObj_ = Object3d::Create();
	planeObj_->SetModel(planeModel_);
	planeObj_->SetCamera(camera_);

	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetCamera(camera_);

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//描画基盤
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "title.png");

	//描画スプライト

	sprite_->Initialize(drawBas_,0);

	//パーティクルマネージャー
	particleManager_ = ParticleManager::Create();
	particleManager_->LoadTexture(0, "particle.png");
	particleManager_->SetTextureIndex(0);
	particleManager_->SetCamera(camera_);
}

void TitleScene::Update(){
	Input::GetInstance()->Update();
	// カメラ移動
	if (Input::GetInstance()->PressKey(DIK_W) ||
		Input::GetInstance()->PressKey(DIK_S) ||
		Input::GetInstance()->PressKey(DIK_D) ||
		Input::GetInstance()->PressKey(DIK_A)) {
		if (Input::GetInstance()->PressKey(DIK_W)) {
			camera_->MoveVector({ 0.0f,+1.0f,0.0f });
		}
		else if (Input::GetInstance()->PressKey(DIK_S)) {
			camera_->MoveVector({ 0.0f,-1.0f,0.0f });
		}
		if (Input::GetInstance()->PressKey(DIK_D)) {
			camera_->MoveVector({ +1.0f,0.0f,0.0f });
		}
		else if (Input::GetInstance()->PressKey(DIK_A)) {
			camera_->MoveVector({ -1.0f,0.0f,0.0f });
		}
		camera_->Update();
	}

	light_->Update();

	skydomeObj_->Update();
	planeObj_->Update();

	object1->Update();

	sprite_->Update();

	if (particleNum_ >= 100) {
		particleNum_ = 0;
	}

	particleNum_++;

	for (int i = 0; i < 100; i++) {

		if (i == particleNum_) {
			particleManager_->Config(10.0f, 0.1f, 0.001f, 1.0f, 256.0f);
		}
	}

	particleManager_->Update();

	if (input_->TriggerKey(DIK_RETURN)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

void TitleScene::Draw(){
	//FBX描画
	object1->Draw(dxBas_->GetCommandList().Get());


	// パーティクル描画前処理
	ParticleManager::PreDraw(dxBas_->GetCommandList().Get());

	// パーティクルの描画
	particleManager_->Draw();

	// パーティクル描画後処理
	ParticleManager::PostDraw();

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	//skydomeObj_->Draw();
	//planeObj_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	sprite_->Draw();

	drawBas_->PostDraw();
}

void TitleScene::Finalize(){
	SafeDelete(planeObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);
	SafeDelete(particleManager_);

	SafeDelete(light_);
	SafeDelete(camera_);

	SafeDelete(object1);
	SafeDelete(model1);

}