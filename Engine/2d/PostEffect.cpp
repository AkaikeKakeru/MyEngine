﻿#include "PostEffect.h"
#include "WinApp.h"
#include <cassert> 
#include <MyMath.h>

//静的メンバ変数の実体//

//クリアカラー
const float PostEffect::clearColor_[4] = {
	0.25f,//R
	0.5f,//G
	0.1f,//B
	0.0f//A
};//緑っぽい色

void PostEffect::Initialize(uint32_t textureIndex) {
	device_ = spriteBas_->GetDevice();
	cmdList_ = spriteBas_->GetCommandList();

	//スプライト初期化
	{
		//テクスチャサイズをイメージに合わせる
		if (textureIndex != UINT32_MAX) {
			textureIndex_ = textureIndex;
			AdjustTextureSize();
			//テクスチャサイズをスプライトのサイズに適用
			size_ = textureSize_;
		}
		anchorPoint_ = { 0.5f,0.5f };

		worldTransform_.scale = { 1,1,1 };
		worldTransform_.rotation = ConvertToRadian(0.0f);
		worldTransform_.position = { 
			WinApp::Win_Width / 2.0f,
			WinApp::Win_Height / 2.0f
		};
		worldTransform_.matWorld = Matrix4Identity();

		matOrtGrapricProjection_ = Matrix4Identity();
		//テクスチャの左上を、画面の左上角に合わせたい
		//ポリゴンの左上を、画面中央に合わせる
		matOrtGrapricProjection_.m[0][0] = 2.0f / WinApp::Win_Width;
		matOrtGrapricProjection_.m[1][1] = -2.0f / WinApp::Win_Height;
		//上の状態から、画面半分くらいの距離だけ、左上にずらす
		matOrtGrapricProjection_.m[3][0] = -1.0f;
		matOrtGrapricProjection_.m[3][1] = 1.0f;

		Sprite::CreateVertexBufferView();
		GenerateConstBuffer();
	}

	//テクスチャバッファ生成
	GenerateTextureBuffer();

	//テクスチャバッファへ転送
	TransferTextureBuffer();

	//SRV作成
	CreateSRV();

	//RTV作成
	CreateRTV();

	//深度バッファ生成
	GenerateDepthBuffer();

	//DSV作成
	CreateDSV();
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

void PostEffect::GenerateConstBuffer() {
	GenerateConstMaterial();
	GenerateConstTransform();
}

void PostEffect::GenerateConstMaterial() {
	HRESULT result;

	//定数バッファヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//定数バッファリソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256バイトアライメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);//マッピング
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される

	//色情報をGPUに転送
	constMapMaterial_->color = color_;
}

void PostEffect::GenerateConstTransform() {
	HRESULT result;

	//定数バッファヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
											 //定数バッファリソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256バイトアライメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);//マッピング
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	//ワールド行列を再計算
	ReCalcMatWorld();

	//ワールド変換行列と、平行投影変換行列を掛ける
	constMapTransform_->mat = worldTransform_.matWorld *= matOrtGrapricProjection_;
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

void PostEffect::PreDrawScene() {
	//リソースバリアデスク設定(シェーダーリソースから、描画可能状態に)
	barrierDesc_.Transition.pResource = texBuff_.Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	//リソースバリアを変更
	cmdList_->ResourceBarrier(1, &barrierDesc_);

	//レンダ―ターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV_->GetCPUDescriptorHandleForHeapStart();

	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart();

	//レンダ―ターゲットをセット
	cmdList_->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//ビューポートの設定
	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::Win_Width;
	viewport.Height = WinApp::Win_Height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポート設定コマンドを、コマンドリストに積む
	cmdList_->RSSetViewports(1, &viewport);

	//シザー矩形の設定
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;										//切り抜き座標左
	scissorRect.right = scissorRect.left + WinApp::Win_Width;	//切り抜き座標右
	scissorRect.top = 0;										//切り抜き座標上
	scissorRect.bottom = scissorRect.top + WinApp::Win_Height;	//切り抜き座標下
	//シザー矩形設定コマンドを、コマンドリストに積む
	cmdList_->RSSetScissorRects(1, &scissorRect);

	//RTVクリア
	cmdList_->ClearRenderTargetView(rtvH, clearColor_, 0, nullptr);
	//DSVクリア
	cmdList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene() {
	//リソースバリアデスク設定(描画可能状態から、シェーダーリソースに)
	barrierDesc_.Transition.pResource = texBuff_.Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	//リソースバリアを変更
	cmdList_->ResourceBarrier(1, &barrierDesc_);
}

void PostEffect::GenerateTextureBuffer() {
	HRESULT result = 0;

	//テクスチャバッファヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//テクスチャバッファのクリア設定
	D3D12_CLEAR_VALUE texClearValue{};
	texClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (size_t i = 0; i < 4; i++) {
		texClearValue.Color[i] = clearColor_[i];
	}

	//テクスチャリソース設定
	D3D12_RESOURCE_DESC texResDesc{};
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texResDesc.Width = WinApp::Win_Width;
	texResDesc.Height = (UINT)WinApp::Win_Height;
	texResDesc.DepthOrArraySize = 1;
	texResDesc.MipLevels = 0;
	texResDesc.SampleDesc.Count = 1;
	texResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

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
}

void PostEffect::TransferTextureBuffer() {
	HRESULT result = 0;

	//テクスチャを赤でクリア//

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

void PostEffect::CreateSRV() {
	HRESULT result = 0;

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

void PostEffect::CreateRTV() {
	HRESULT result = 0;

	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;

	//RTV用デスクリプタヒープ生成
	result = device_->CreateDescriptorHeap(
		&rtvDescHeapDesc,
		IID_PPV_ARGS(&descHeapRTV_)
	);
	assert(SUCCEEDED(result));

	//レンダ―ターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//シェーダーの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//レンダ―ターゲットビューの生成
	device_->CreateRenderTargetView(
		texBuff_.Get(),
		&rtvDesc,
		descHeapRTV_->GetCPUDescriptorHandleForHeapStart()
	);
}

void PostEffect::GenerateDepthBuffer() {
	HRESULT result = 0;

	//深度リソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WinApp::Win_Width;
	depthResDesc.Height = WinApp::Win_Height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	//リソース生成
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
}

void PostEffect::CreateDSV() {
	HRESULT result = 0;

	//深度ビュー用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc = {};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescHeapDesc.NumDescriptors = 1;

	//DSV用デスクリプタヒープを生成
	result = device_->CreateDescriptorHeap(
		&dsvDescHeapDesc,
		IID_PPV_ARGS(&descHeapDSV_)
	);
	assert(SUCCEEDED(result));

	//深度ビュー設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	//デスクリプタヒープにDSV作成
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart()
	);
}

PostEffect::PostEffect()
	:Sprite(
		100,
		{ 0, 0 },
		{ 500.0f, 500.0f },
		{ 1,1,1,1 },
		{ 0.5f, 0.5f },
		false,
		false) {
}
