#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <wrl.h>
#include "Vector2.h"
#include <d3d12.h>
#include <d3dx12.h>

class Model {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://構造体
	// 頂点データ構造体
	struct VertexPosNormalUv {
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataMaterial {
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

public://関数
	static Model* LoadFromOBJ(const std::string& modelname);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <returns>成否</returns>
	void LoadTexture(const std::string& directoryPath, const std::string filename);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);

private: // 非公開メンバ関数
	void LoadFromOBJInternal(const std::string& modelname);
	//デスクリプタヒープの初期化
	void InitializeDescriptorHeap();
	//バッファ生成
	void CreateBaffers();

public://セッタ
	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }

private:
	//デバイス
	static ComPtr<ID3D12Device> device_;
	// デスクリプタサイズ
	static UINT descriptorIncrementSize_;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// シェーダリソースビューのハンドル(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuffMaterial_;

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;
	//マテリアル
	Material material_;
};