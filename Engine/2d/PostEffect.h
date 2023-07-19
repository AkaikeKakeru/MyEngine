#pragma once
#include "Sprite.h"
class PostEffect :
	public Sprite {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数
	//初期化
	void Initialize(uint32_t textureIndex);
	//描画コマンド実行
	void Draw();

	//テクスチャ描画事前処理
	void PreDraw();
	//テクスチャセットコマンド
	void TextureCommand();

	//シーン描画前処理
	void PreDrawScene();
	//シーン描画後処理
	void PostDrawScene();

	//テクスチャバッファ生成
	void GenerateTextureBuffer();
	//テクスチャバッファへ転送
	void TransferTextureBuffer();
	//SRV作成
	void CreateSRV();
	//RTV作成
	void CreateRTV();
	//深度バッファ生成
	void GenerateDepthBuffer();
	//DSV作成
	void CreateDSV();

private://静的メンバ変数
	//画面クリアカラー
	static const float clearColor_[4];

private://メンバ変数
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;
	//シェーダーリソースビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff_;
	//レンダ―ターゲットビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
	//深度ステンシル用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV_;
	
	//リソースバリアデスク
	D3D12_RESOURCE_BARRIER barrierDesc_{};

public://コンストラクタ
	PostEffect();
};
