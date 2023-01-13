#pragma once
#include "Vector3.h"
#include <d3dx12.h>
#include <wrl.h>

class Material {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	// 定数バッファ用データ構造体B1
	struct ConstBufferDataMaterial {
		Vector3 ambient;	//アンビエント係数
		float pad1;			//パディング
		Vector3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		Vector3 specular;	//スペキュラー係数
		float alpha;		//アルファ
	};

public:
	void Initialize();

	//デスクリプタヒープの初期化
	//void InitializeDescriptorHeap();

	/// テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath, const std::string filename, UINT incrementSize);

	/// マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename, UINT incrementSize);

	void CreateBaffer();

	ID3D12Resource* GetConstBuffer() const { return constBuffMaterial_.Get(); }

	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descHeap_.Get(); }

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescHandleSRV() const { return gpuDescHandleSRV_; }

	std::string GetTextureFileName()const { return textureFilename_; };

	const void SetMaterialName(std::string name) { name_ = name; } //マテリアル名
	const void SetAmbient(Vector3 ambient) { ambient_ = ambient; } //アンビエント影響度
	const void SetDiffuse(Vector3 diffuse) { diffuse_ = diffuse; } //ディフューズ影響度
	const void SetSpecular(Vector3 specular) { specular_ = specular; } //スペキュラー影響度
	const void SetAlpha(float alpha) { alpha_ = alpha; } //アルファ
	const void SetTextureFilename(std::string textureFilename) { textureFilename_ = textureFilename; } //テクスチャファイル名
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

	// 定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuffMaterial_;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;

	std::string name_; //マテリアル名
	Vector3 ambient_; //アンビエント影響度
	Vector3 diffuse_; //ディフューズ影響度
	Vector3 specular_; //スペキュラー影響度
	float alpha_; //アルファ
	std::string textureFilename_; //テクスチャファイル名
};