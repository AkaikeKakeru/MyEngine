#include "PostEffect.h"
#include "WinApp.h"
#include <cassert> 
#include <d3dx12.h>

void PostEffect::Initialize() {
	HRESULT result = 0;

	//スプライト初期化
	Sprite::Initialize();

	//テクスチャバッファヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//テクスチャリソース設定
	D3D12_RESOURCE_DESC texResDesc{};
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Width = WinApp::Win_Width;
	texResDesc.Height = (UINT)WinApp::Win_Height;
	texResDesc.DepthOrArraySize = 1;
	texResDesc.MipLevels = 0;
	texResDesc.SampleDesc.Count = 1;
	texResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//テクスチャバッファ生成
	result = device_->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff_));

	assert(SUCCEEDED(result));
}

void PostEffect::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	//描画前処理
	spriteBas_->PreDraw();

	//描画用テクスチャコマンド
	spriteBas_->SetTextureCommand(textureIndex_);

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

PostEffect::PostEffect()
	:Sprite(
		100,
		{ 0, 0 },
		{ 500.0f, 500.0f },
		{ 1,1,1,1 },
		{ 0.0f, 0.0f },
		false,
		false) {
}
