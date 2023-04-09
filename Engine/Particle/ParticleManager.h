#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <forward_list>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

#include "Camera.h"

#include <string>
#include <array>

/// <summary>
/// 3Dオブジェクト
/// </summary>
class ParticleManager {
private: //省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPos {
		Vector3 pos_; // xyz座標
		float scale_ = 0.0f; //スケール
		Vector4 color_; //色
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Matrix4 mat_;	// ３Ｄ変換行列
		Matrix4 matBillboard_; //ビルボード行列
		Vector4 color_; //色
	};

	//パーティクル一粒
	struct Particle{
		//座標
		Vector3 position_ = {};
		//速度
		Vector3 velocity_ = {};
		//加速度
		Vector3 accel_ = {};

		//スケール
		float scale_ = 1.0f;
		//スケール初期値
		float s_scale_ = 1.0f;
		//スケール最終値
		float e_scale_ = 0.0f;

		//色
		Vector4 color_ = {};
		//色初期値
		Vector4 s_color_ = {};
		//色最終値
		Vector4 e_color_ = {};

		//現在フレーム
		int frame_ = 0;
		//終了フレーム
		int num_frame_ = 0;
	};

private: // 定数
	static const float radius_;				// 底面の半径
	static const float prizmHeight_;			// 柱の高さ
	static const int vertexCount_ = 1024;		// 頂点数
	//SRVの最大個数
	static const size_t kMaxSRVCount = 2056;
	//デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDhirectoryPath;
public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

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
	static ParticleManager* Create();

private: // 静的メンバ変数
		 // デバイス
	static ID3D12Device* device_;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate_;
	// 頂点バッファ
	static ComPtr<ID3D12Resource> vertBuff_;

	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView_;
	// 頂点データ配列
	static VertexPos vertices_[vertexCount_];

public: //メンバ変数
	//パーティクル配列
	std::forward_list<Particle> particles_;

private:// 静的メンバ関数
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	static void InitializeDescriptorHeap();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// モデル作成
	/// </summary>
	static void CreateModel();

public: //メンバ関数
	
		/// <summary>
		/// テクスチャ読み込み
		/// </summary>
	void LoadTexture(uint32_t textureIndex, const std::string& fileName);


	/// <summary>
	/// パーティクルの追加
	/// </summary>
	/// <param name="life">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール</param>
	/// <param name="end_scale">終了時スケール</param>
	/// <param name="start_color">開始時色</param>
	/// <param name="end_color">終了時色</param>
	void Add(int life, 
		Vector3 position, Vector3 velocity, Vector3 accel,
		float start_scale,float end_scale,
		Vector4 start_color, Vector4 end_color);

	//配置
	void Config(
		float rnd_pos = 10.0f,
		float rnd_vel = 0.1f,
		float rnd_acc = 0.001f,
		float rnd_scale = 1.0f,
		float rnd_color = 256.0f);

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

	void SetCamera(Camera* camera) {
		camera_ = camera;
	}

	void SetTextureIndex(uint32_t index) {
		textureIndex_ = index;
	}
private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff_; // 定数バッファ
	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVCount> texBuffs_;
	//テクスチャリソースデスク
	D3D12_RESOURCE_DESC texResDesc_{};
	//デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> srvHeap_;
	//SRVヒープのCPUアドレス
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle_;
	//SRVヒープのGPUアドレス
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_;
	//テクスチャ格納ディレクトリ
	static std::string textureDhirectoryPath_;

	// ローカルスケール
	Vector3 scale_ = { 1,1,1 };

	Camera* camera_ = nullptr;

	uint32_t textureIndex_;
};