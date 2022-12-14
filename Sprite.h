#pragma once
#include "DrawBasis.h"
#include "Vector3.h"

class Sprite {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体
	typedef struct ConstBufferDataMaterial {
		Vector4 color;//色(RGBA)
	};

	typedef struct VertexPos {
		Vector3 pos;
	} VertexPos;

public://基本関数
	void Initialize(DrawBasis* drawBas);
	void Draw();

private://固有関数
	//定数バッファ生成
	void GenerateConstBuffer();

private://定数
	//頂点数
	static const int kVerticesNum = 3;

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
};