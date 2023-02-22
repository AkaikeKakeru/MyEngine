﻿#pragma once

#include <Windows.h>
#include <wrl.h>

#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Model.h"

#include "WorldTransform.h"

#include "Camera.h"
#include "Light.h"

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d {
private: // エイリアス
		 // Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
		// パイプラインセット
	struct PipelineSet {
		// ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature_;
		// パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate_;
	};

private: // 定数
	static const int kRootParamCount_ = 4;

public: // 静的メンバ関数
		// 静的初期化
	static void StaticInitialize(ID3D12Device* device, Camera* camera = nullptr);

	// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	// 描画後処理
	static void PostDraw();

	// 3Dオブジェクト生成
	static Object3d* Create();

	static void SetLight(Light* light) {
		Object3d::light_ = light;
	}

	//カメラセット
	static void SetCamera(Camera* camera) {
		camera_ = camera;
	}

private: // 静的メンバ変数
		 // デバイス
	static ID3D12Device* device_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature_;
	// テクスチャあり用パイプライン
	static PipelineSet pipelineSet_;

	//カメラ
	static Camera* camera_;

	//ライト
	static Light* light_;

private:// 静的メンバ関数
		// グラフィックパイプライン生成
	static void InitializeGraphicsPipeline();

public: // メンバ関数
	bool Initialize();
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();

	//転送
	void TransferMatrixWorld();

	// 座標の取得
	const Vector3& GetPosition() const { return worldTransform_.position_; }

	//スケールの取得
	const Vector3& GetScale() const { return worldTransform_.scale_; }

	//回転の取得
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }

	/// 座標の設定
	void SetPosition(const Vector3& position) { this->worldTransform_.position_ = position; }

	//スケールの設定
	void SetScale(const Vector3& scale) { this->worldTransform_.scale_ = scale; }

	//回転の設定
	void SetRotation(const Vector3& rotation) { this->worldTransform_.rotation_ = rotation; }

	//ワールド変換の設定
	void SetWorldTransform(const WorldTransform worldTransform) { worldTransform_ = worldTransform; }

	//モデルセッタ
	void SetModel(Model* model) { model_ = model; }

private: // メンバ変数
		 //ワールドトランスフォーム
	WorldTransform worldTransform_;

	//モデル
	Model* model_ = nullptr;
};