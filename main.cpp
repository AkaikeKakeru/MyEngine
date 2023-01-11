#include <Windows.h>
#include "SafeDelete.h"
#include "WinApp.h"
#include "DirectXBasis.h"
#include "Input.h"
#include "ObjectBasis.h"
#include "DrawBasis.h"
#include "Model.h"
#include "Sprite.h"
#include "Degree.h"
#include "Object3d.h"

//マウスボタン
typedef enum MouseButtonNum {
	LeftButton,
	RightButton,
	CenterButton,
}MouseButtonNum;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	///基盤初期化
	//アプリケーション
	WinApp* winApp_ = nullptr;
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize();

	//DirectX基盤
	DirectXBasis* dxBas_ = nullptr;
	dxBas_ = DirectXBasis::GetInstance();
	dxBas_->Initialize(winApp_);

	//Input
	Input* input_ = nullptr;
	input_ = Input::GetInstance();
	input_->Initialize();

	/// 描画初期化

	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get(), WinApp::Win_Width, WinApp::Win_Height);

	//オブジェクトモデル
	Model* model_ = Model::LoadFromOBJ("cube");
	Model* model_2 = Model::LoadFromOBJ("triangle_mat");
	
	Model* modelSkydome_ = Model::LoadFromOBJ("skydome");
	Model* modelGround_ = Model::LoadFromOBJ("ground");

	Object3d* object3d_ = Object3d::Create();
	Object3d* object3d_2 = Object3d::Create();

	Object3d* skydome_ = Object3d::Create();

	Object3d* ground_ = Object3d::Create();

	object3d_->SetModel(model_);
	object3d_2->SetModel(model_2);

	skydome_->SetModel(modelSkydome_);
	ground_->SetModel(modelGround_);

	object3d_->SetPosition({ -5.0f, 0.0f, -5.0f });
	object3d_->SetPosition({ +5.0f, 0.0f, +5.0f });

	//描画基盤
	DrawBasis* drawBas_ = nullptr;
	drawBas_ = DrawBasis::GetInstance();
	drawBas_->Initialize();

	drawBas_->LoadTexture(0, "smile.png");

	//描画スプライト
	//Sprite* sprite_ = new Sprite();
	//sprite_->Initialize(drawBas_);

	//Sprite* sprite2_ = new Sprite();
	//sprite2_->Initialize(drawBas_);

	//Sprite* sprite3_ = new Sprite();
	//sprite3_->Initialize(drawBas_);

	/// ゲームループ
	while (true) {
		//windowsのメッセージ処理
		if (winApp_->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}

		/// 更新
		input_->Update();

		Vector2 pos = { 0,0 };

		//オブジェクト更新

		 //オブジェクト移動
		if (input_->PressKey(DIK_UP) || input_->PressKey(DIK_DOWN) || input_->PressKey(DIK_RIGHT) || input_->PressKey(DIK_LEFT)){
			// 現在の座標を取得
			Vector3 position = object3d_->GetPosition();

			// 移動後の座標を計算
			if (input_->PressKey(DIK_UP)) { position.y += 1.0f; }
			else if (input_->PressKey(DIK_DOWN)) { position.y -= 1.0f; }
			if (input_->PressKey(DIK_RIGHT)) { position.x += 1.0f; }
			else if (input_->PressKey(DIK_LEFT)) { position.x -= 1.0f; }

			// 座標の変更を反映
			object3d_->SetPosition(position);
		}

		// カメラ移動
		if (input_->PressKey(DIK_W) || input_->PressKey(DIK_S) || input_->PressKey(DIK_D) || input_->PressKey(DIK_A)){
			if (input_->PressKey(DIK_W)) { Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f }); }
			else if (input_->PressKey(DIK_S)) { Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f }); }
			if (input_->PressKey(DIK_D)) { Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f }); }
			else if (input_->PressKey(DIK_A)) { Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f }); }
		}

		object3d_->Update();
		object3d_2->Update();

		//pos = sprite_->GetPosition();
		//if (input_->PressMouse(LeftButton)) {
		//	pos.x += 2.0f;
		//}
		//sprite_->SetPosition(pos);
		//sprite_->SetRotation(ConvertToRadian(45.0f));
		//sprite_->SetColor(Vector4(0.5f, 1.0f, 0.2f, 0.4f));
		//sprite_->SetSize(Vector2(80, 120));
		//sprite_->SetIsFlipX(true);
		//sprite_->Update();

		//pos = sprite2_->GetPosition();
		//if (input_->TriggerMouse(CenterButton)
		//	|| input_->PressMouse(RightButton)) {
		//	pos.y += 2.0f;
		//}
		//sprite2_->SetPosition(pos);
		//sprite2_->SetColor(Vector4(0.8f, 0.2f, 0.5f, 0.9f));
		//sprite2_->SetSize(Vector2(140, 50));
		//sprite2_->SetIsFlipY(true);
		//sprite2_->Update();

		//sprite3_->SetPosition(input_->GetMousePosition());
		//sprite3_->SetAnchorPoint(Vector2(0.5f, 0.5f));
		//sprite3_->Update();

		/// 描画
		//描画前処理
		dxBas_->PreDraw();

		//モデル本命処理
		Object3d::PreDraw(dxBas_->GetCommandList().Get());
		
		skydome_->Draw();
		ground_->Draw();

		object3d_->Draw();
		object3d_2->Draw();
		
		Object3d::PostDraw();

		//objectBas_->PreDraw();
		//model_->Draw();
		//objectBas_->PostDraw();

		//スプライト本命処理
		drawBas_->PreDraw();

		//sprite_->Draw();
		//sprite2_->Draw();
		//sprite3_->Draw();

		drawBas_->PostDraw();

		//描画後処理
		dxBas_->PostDraw();
	}

	/// 解放
	SafeDelete(skydome_);
	SafeDelete(ground_);
	SafeDelete(object3d_);
	SafeDelete(object3d_2);
	SafeDelete(model_);
	SafeDelete(model_2);

	//SafeDelete(sprite_);
	//SafeDelete(sprite2_);
	//SafeDelete(sprite3_);

	//SafeDelete(drawBas_);
	//SafeDelete(dxBas_);
	//SafeDelete(winApp_);

	return 0;
}