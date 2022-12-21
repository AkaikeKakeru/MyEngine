#pragma once
#include "DirectXBasis.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <string>
#include <array>

class ObjectBasis {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体

public://基本関数
	void Initialize();
	void PreDraw();
	void PostDraw();

public://publicな固有関数
	  //テクスチャ読み込み
	void LoadTexture(uint32_t textureIndex, const std::string& fileName);
	//描画用テクスチャコマンド
	void SetTextureCommand(uint32_t textureIndex);

private://固有関数
	/// <summary>
	/// パイプライン
	/// </summary>

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

	/// <summary>
	/// テクスチャ
	/// </summary>

	//テクスチャバッファ生成
	void GenerateTextureBuffer(uint32_t textureIndex,const wchar_t* wfileName);
	//デスクリプタヒープ生成
	void GenerateDescriptorHeap();
	//シェーダーリソースビュー作成
	void CreateShaderResourceView(uint32_t textureIndex);

private://定数
	//頂点レイアウトの要素数
	static const int kInputLayoutElement = 2;
	//ルートパラメータ数
	static const int kRootParamCount = 3;

	//SRVの最大個数
	static const size_t kMaxSRVCount = 2056;
	//デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDhirectoryPath;

private://メンバ変数
	//Draw基盤
	DirectXBasis* dxBas_ = nullptr;

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	/// <summary>
	/// パイプライン
	/// </summary>

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

	/// <summary>
	/// テクスチャ
	/// </summary>

	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>,kMaxSRVCount> texBuffs_;
	//テクスチャリソースデスク
	D3D12_RESOURCE_DESC texResDesc_{};
	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;
	//SRVヒープのアドレス
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
	//インクリメントサイズ
	UINT incrementSize_;
	//テクスチャ格納ディレクトリ
	std::string textureDhirectoryPath_;

public://ゲッタ
	ComPtr<ID3D12Device> GetDevice() { return device_; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return cmdList_; }

private:
	ObjectBasis() = default;
	~ObjectBasis() = default;
	//コピーコンストラクタを無効
	ObjectBasis(const ObjectBasis&) = delete;
	//代入演算子を無効
	const ObjectBasis& operator=(const ObjectBasis&) = delete;

public://static
	static ObjectBasis* GetInstance();
};