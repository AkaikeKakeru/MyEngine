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

public://コンストラクタ
	PostEffect();
};
