#include "TitleScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();

void TitleScene::Initialize() {
	/// 描画初期化
	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get());

	//オブジェクトモデル

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1.0f,1.0f,1.0f });
	Object3d::SetLight(light_);

	//カメラ生成
	camera_ = new Camera();

	//描画基盤
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "title.png");

	//描画スプライト

	sprite_->Initialize(drawBas_, 0);


	//連結してフルパスを得る
	const std::string fullpath =
		std::string("Resource/levels/") + "rimuta_TL1" + ".json";

	loader_ = new Loader();

	loader_->LoadFileJson("rimuta_TL1", camera_);

	objects_ = loader_->GetObjects();
}

void TitleScene::Update() {
	input_->Update();

	light_->Update();

	for (Object3d* object : objects_) {
		object->Update();
	}
}

void TitleScene::Draw() {
	// パーティクル描画前処理
	ParticleManager::PreDraw(dxBas_->GetCommandList().Get());


	// パーティクル描画後処理
	ParticleManager::PostDraw();

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	for (Object3d* object : objects_) {
		object->Draw();
	}

	Object3d::PostDraw();

	//スプライト本命処理
	drawBas_->PreDraw();

	drawBas_->PostDraw();
}

void TitleScene::Finalize() {

	SafeDelete(light_);
	SafeDelete(camera_);

	//for (Object3d* object : objects_) {
	//	SafeDelete(object);
	//}

	loader_->Finalize();
	SafeDelete(loader_);
}