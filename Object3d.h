#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Model.h"

#include "WorldTransform.h"
#include "ViewProjection.h"

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d {
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv {
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0 {
		//Vector4 color;	// 色 (RGBA)
		Matrix4 mat;	// ３Ｄ変換行列
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1 {
		Vector3 ambient;	//アンビエント係数
		float pad1;			//パディング
		Vector3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		Vector3 specular;	//スペキュラー係数
		float alpha;		//アルファ
	};

	//マテリアル
	struct Material {
		std::string name; //マテリアル名
		Vector3 ambient; //アンビエント影響度
		Vector3 diffuse; //ディフューズ影響度
		Vector3 specular; //スペキュラー影響度
		float alpha; //アルファ
		std::string textureFilename; //テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};
private: // 定数
	static const int division = 50;					// 分割数
	static const float radius;				// 底面の半径
	static const float prizmHeight;			// 柱の高さ
	static const int planeCount = division * 2 + division * 2;		// 面の数
	static const int vertexCount = planeCount * 3;		// 頂点数

	//static const int vertexCount = 3;		// 頂点数
	static const int indexCount = 3 * 2;		// インデックス数

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(ID3D12Device* device, int window_width, int window_height);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static Object3d* Create();

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	static const Vector3& GetEye() { return viewProjection_.camera_.eye_; }

	/// <summary>
	/// 視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	static void SetEye(Vector3 eye);

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	static const Vector3& GetTarget() { return viewProjection_.camera_.target_; }

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	static void SetTarget(Vector3 target);

	/// <summary>
	/// ベクトルによる移動
	/// </summary>
	/// <param name="move">移動量</param>
	static void CameraMoveVector(Vector3 move);

	/// <summary>
	/// ベクトルによる視点移動
	/// </summary>
	/// <param name="move">移動量</param>
	static void CameraMoveEyeVector(Vector3 move);

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;

	//ビュープロジェクション
	static ViewProjection viewProjection_;

private:// 静的メンバ関数
	/// <summary>
	/// カメラ初期化
	/// </summary>
	/// <param name="window_width">画面横幅</param>
	/// <param name="window_height">画面縦幅</param>
	static void InitializeCamera(int window_width, int window_height);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	static void UpdateViewMatrix();

public: // メンバ関数
	bool Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const Vector3& GetPosition() const { return worldTransform_.position_; }

	//スケールの取得
	const Vector3& GetScale() const { return worldTransform_.scale_; }

	//回転の取得
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }

	//ビュープロジェクションの取得
	const ViewProjection& GetViewProjection() const { return viewProjection_; }

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const Vector3& position) { this->worldTransform_.position_ = position; }

	//スケールの設定
	void SetScale(const Vector3& scale) { this->worldTransform_.scale_ = scale; }

	//回転の設定
	void SetRotation(const Vector3& rotation) { this->worldTransform_.rotation_ = rotation; }

	//ワールド変換の設定
	void SetWorldTransform(const WorldTransform worldTransform) { worldTransform_ = worldTransform; }

	//ビュープロジェクションの設定
	void SetViewProjection(const ViewProjection& viewProjection) { viewProjection_ = viewProjection; }

	//モデルセッタ
	void SetModel(Model* model) { model_ = model; }

private: // メンバ変数
	// 色
	Vector4 color = { 1,1,1,1 };

	//ワールドトランスフォーム
	WorldTransform worldTransform_;

	//モデル
	Model* model_ = nullptr;
};