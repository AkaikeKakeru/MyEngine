#pragma once
#include "DirectXBasis.h"

class DrawBasis {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://基本関数
	void Initialize();

private://固有関数
	//頂点バッファビュー作成
	void CreateVertexBufferView();
	//シェーダーファイルのコンパイル
	void CompileShaderFile();
	//頂点レイアウト
	void AssembleVertexLayout();
	//グラフィックスパイプライン
	void AssembleGraphicsPipeline();

private://定数
	static const int kInputLayoutElement = 1;

private://メンバ変数
	DirectXBasis* dxBas_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_;//ピクセルシェーダオブジェクト

	D3D12_INPUT_ELEMENT_DESC inputLayout_[kInputLayoutElement];//頂点レイアウト
};