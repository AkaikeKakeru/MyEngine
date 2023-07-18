#include "PostEffect.h"
#include "WinApp.h"
#include <cassert> 
#include <d3dx12.h>

void PostEffect::Initialize(uint32_t textureIndex) {
	HRESULT result = 0;

	//スプライト初期化
	Sprite::Initialize(textureIndex);

	//テクスチャバッファヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//テクスチャリソース設定
	D3D12_RESOURCE_DESC texResDesc{};
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		texResDesc.Width = WinApp::Win_Width;
	texResDesc.Height = (UINT)WinApp::Win_Height;
	texResDesc.DepthOrArraySize = 1;
	texResDesc.MipLevels = 0;
	texResDesc.SampleDesc.Count = 1;

	//テクスチャバッファ生成
	result = device_->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff_)
	);
	assert(SUCCEEDED(result));

	{//テクスチャを赤でクリア
		//画素数(1200 * 720 = 921600ピクセル)
		const UINT pixcelCount =
			WinApp::Win_Width * WinApp::Win_Height;
		//画像1行分のデータサイズ
		const UINT rowPitch =
			sizeof(UINT) * WinApp::Win_Width;
		//画像全体のデータサイズ
		const UINT depthPitch = rowPitch * WinApp::Win_Height;
		//画像イメージ
		UINT* img = new UINT[pixcelCount];
		for (int i = 0; i < pixcelCount; i++) {
			img[i] = 0xff0000ff;
		}

		//テクスチャバッファにデータ転送
		result = texBuff_->WriteToSubresource(
			0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}

	//SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	//SRV用デスクリプタヒープを生成
	result = device_->CreateDescriptorHeap(
		&srvDescHeapDesc,
		IID_PPV_ARGS(&descHeapSRV_)
	);
	assert(SUCCEEDED(result));

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	//デスクリプタヒープにSRV作成
	device_->CreateShaderResourceView(
		texBuff_.Get(),
		&srvDesc,
		descHeapSRV_->GetCPUDescriptorHandleForHeapStart()
	);
}

void PostEffect::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	//描画前処理//
	PreDraw();

	//描画処理//

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//グラフィックスルートデスクリプタテーブルの設定コマンド
	TextureCommand();

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

void PostEffect::PreDraw() {
	//パイプラインステートとルートシグネイチャの設定コマンド
	cmdList_->SetPipelineState(spriteBas_->GetPipelineState().Get());
	cmdList_->SetGraphicsRootSignature(spriteBas_->GetRootSignature().Get());

	//プリミティブ形状の設定コマンド
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//三角形ストリップ								   //デスクリプタヒープの配列をセットするコマンド

	//デスクリプタヒープ設定コマンド
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV_.Get() };
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void PostEffect::TextureCommand() {
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle =
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart();

	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
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
