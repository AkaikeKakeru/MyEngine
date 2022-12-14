#pragma once
#include "DirectXBasis.h"
#include "Vector3.h"
#include "Vector4.h"

class DrawBasis {
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
	//頂点数
	static const int kVerticesNum = 3;
	//頂点レイアウトの要素数
	static const int kInputLayoutElement = 1;

private://メンバ変数
	//Draw基盤
	DirectXBasis* dxBas_ = nullptr;

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
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
	//パイプラインステート
	ComPtr<ID3D12PipelineState> pipelineState_;

public://ゲッタ
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return cmdList_; }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vbView_; }

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