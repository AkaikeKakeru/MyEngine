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

	void PreDraw();
	void TextureCommand();

private://メンバ変数
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

public://コンストラクタ
	PostEffect();
};
