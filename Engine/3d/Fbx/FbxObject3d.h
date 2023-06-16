#pragma once

#include "FbxModel.h"
#include "Camera.h"
#include "WorldTransform.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <string>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "MyMath.h"

#include "LightGroup.h"

class FbxObject3d {
protected: //省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //サブクラス

public: //静的メンバ関数
	//グラフィックスパイプライン生成
	static void CreateGraphicsPipeline();

public: //メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//ワールド行列転送
	void TransferMatrixWorld();

public: //アクセッサ
	//デバイスセット
	static void SetDevice(ID3D12Device* device) {
		FbxObject3d::device_ = device;
	}
	//カメラセット
	static void SetCamera(Camera* camera) {
		FbxObject3d::camera_ = camera;
	}

	//ライトセット
	static void SetLight(LightGroup* lightGroup) {
		FbxObject3d::lightGroup_ = lightGroup;
	}

	//モデルセット
	void SetModel(FbxModel* model) {
		model_ = model;
	}

private: //静的メンバ変数
	//デバイス
	static ID3D12Device* device_;

	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature_;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate_;

	//カメラ
	static Camera* camera_;

	//ライト
	static LightGroup* lightGroup_;

protected: //メンバ変数
	//ワールド変換
	WorldTransform worldTransform_;

	//モデル
	FbxModel* model_ = nullptr;
};