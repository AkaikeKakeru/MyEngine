#pragma once
#include <array>
#include <d3dx12.h>
#include <string>
#include <wrl.h>

class TextureManager {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://構造体
	struct Texture {
		//リソース
		ComPtr<ID3D12Resource> resource_;
		//srvCpuハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_;
		//srvGpuハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_;
		//名前
		std::string name;
	};

public://関数
	uint32_t Load(const std::string& filename);
	void Initialize(ID3D12Device* device,std::string directoryPath);
	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t textureHandle);
	uint32_t LoadInternal(const std::string& filename);
	//デスクリプタヒープ生成
	void GenerateDescriptorHeap();
	//リセット
	void ResetAll();

	//テクスチャ読み込み
	void LoadTexture(uint32_t textureHandle, const std::string& fileName);
	//テクスチャバッファ生成
	void GenerateTextureBuffer(Texture texture, const wchar_t* wfileName);
	//シェーダリソースビュー作成
	void CreateShaderResourceView(Texture texture,uint32_t textureHandle);

	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex,
		uint32_t textureHandle);

public://定数
	static const UINT kDescriptors = 2056;

private://変数
	// デバイス
	ComPtr<ID3D12Device> device_;
	//インクリメントサイズ
	UINT incrementSize_ = 0u;
	//テクスチャ格納ディレクトリ
	std::string textureDhirectoryPath_;
	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;
	// 次に使うデスクリプタヒープの番号
	uint32_t indexNextDescriptorHeap_ = 0u;
	// テクスチャコンテナ
	std::array<Texture, kDescriptors> textures_;
	//テクスチャリソースデスク
	D3D12_RESOURCE_DESC texResDesc_{};

private://シングルトン化
	//インスタンス取得
	static TextureManager* GetInstance();

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
};