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

public: //定数
	// ボーンの最大数
	static const int MAX_BONES = 32;

public: //サブクラス
	// 定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin {
		Matrix4 bones[MAX_BONES];
	};

	//ルートパラメータEnum
	enum rootParameter {
		rootParam_worldTransform,
		rootParam_material,
		rootParam_texture,
		rootParam_light,
		rootParam_skining
	};


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

	/// <summary>
	/// アニメーション開始
	/// </summary>
	void PlayAnimation();

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

	// 座標の取得
	const Vector3& GetPosition() const {
		return worldTransform_.position_; }
	//スケールの取得
	const Vector3& GetScale() const {
		return worldTransform_.scale_; }
	//回転の取得
	const Vector3& GetRotation() const {
		return worldTransform_.rotation_; }

	/// 座標の設定
	void SetPosition(const Vector3& position) {
		worldTransform_.position_ = position; }
	//スケールの設定
	void SetScale(const Vector3& scale) {
		worldTransform_.scale_ = scale; }
	//回転の設定
	void SetRotation(const Vector3& rotation) { 
		worldTransform_.rotation_ = rotation; }

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

	//定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBuffSkin_;

	//1フレームの時間
	FbxTime frameTime_;
	//アニメーション開始時間
	FbxTime startTime_;
	//アニメーション終了時間
	FbxTime endTime_;
	//現在時刻 (アニメーション)
	FbxTime currentTime_;
	//アニメーション再生中
	bool isPlay_ = false;
};