#pragma once
#include "DirectXBasis.h"

class DrawBasis {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://基本関数
	void Initialize();
	void Draw();

private://固有関数
	//頂点バッファビュー作成
	void CreateVertexBufferView();
	//シェーダーファイルのコンパイル
	void CompileShaderFile();
	//頂点レイアウト組み立て
	void AssembleVertexLayout();
	//グラフィックスパイプライン作成
	void CreateGraphicsPipeline();
	//グラフィックスパイプライン組み立て
	void AssembleGraphicsPipeline();
	//ルートシグネイチャ生成
	void GenerateRootSignature();
	//パイプラインステート生成
	void GeneratePipelineState();

private://定数
	//頂点レイアウトの要素数
	static const int kInputLayoutElement = 1;

private://メンバ変数
	DirectXBasis* dxBas_ = nullptr;
	//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob_;
	//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_;
	//エラーオブジェクト
	ComPtr<ID3DBlob> errorBlob_;
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout_[kInputLayoutElement];
	//グラフィックスパイプラインデスク
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature> rootSignature_;

private:
	DrawBasis() = default;
	~DrawBasis() = default;
	//コピーコンストラクタを無効
	DrawBasis(const DrawBasis&) = delete;
	//代入演算子を無効
	const DrawBasis& operator=(const DrawBasis&) = delete;

public://static
	static DrawBasis* GetInstance();
};