#pragma once
#include "DrawBasis.h"
#include "Matrix4.h"
#include "Vector3.h"

class Sprite {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体
	//定数バッファ用データ構造体(マテリアル)
	typedef struct ConstBufferDataMaterial {
		Vector4 color;//色(RGBA)
	};

	//定数バッファ用データ構造体(3D変換行列)
	typedef struct ConstBufferDataTransform {
		Matrix4 mat;//3D変換行列
	};

public://基本関数
	void Initialize(DrawBasis* drawBas);
	void Draw();

private://固有関数
	//定数バッファ生成
	void GenerateConstBuffer();
	//定数バッファマテリアル生成
	void GenerateConstMaterial();
	//定数バッファトランスフォーム生成
	void GenerateConstTransform();

	//テクスチャバッファ生成
	void GenerateTextureBuffer();
	//デスクリプタヒープ生成
	void GenerateDescriptorHeap();
	//シェーダーリソースビュー作成
	void CreateShaderResourceView();

private://定数
	//頂点数
	static const int kVerticesNum = 4;

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

private://メンバ変数
	//Draw基盤
	DrawBasis* drawBas_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	//定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuffMaterial_;
	//定数バッファマテリアルマップ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	//定数バッファトランスフォーム
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファマテリアルマップ
	ConstBufferDataTransform* constMapTransform_ = nullptr;

	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;

	//テクスチャリソースデスク
	D3D12_RESOURCE_DESC texResDesc_{};

	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;

	//SRVヒープのアドレス
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
};