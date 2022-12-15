#include "Sprite.h"
#include "SafeDelete.h"
#include <cassert>

void Sprite::Initialize(DrawBasis* drawBas) {
	assert(drawBas);
	drawBas_ = drawBas;
	device_ = drawBas_->GetDvice();
	cmdList_ = drawBas_->GetCommandList();
	vbView_ = drawBas_->GetVertexBufferView();

	GenerateConstBuffer();
	GenerateTextureBuffer();
	GenerateDescriptorHeap();
}

void Sprite::Draw() {
	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

void Sprite::GenerateConstBuffer(){
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
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	constMapMaterial->color = Vector4(1, 0, 0, 0.5f);//RGBAで半透明の赤
}

void Sprite::GenerateTextureBuffer(){
	HRESULT result;

	//画像イメージデータ配列
	Vector4* imageData = new Vector4[imageDataCount];//※後で開放する

	//全ピクセルの色を初期化
	for (size_t i = 0; i < imageDataCount; i++){
		imageData[i].x = 1.0f;//R
		imageData[i].y = 0.0f;//G
		imageData[i].z = 0.0f;//B
		imageData[i].w = 1.0f;//A
	}

	//テクスチャバッファヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//テクスチャバッファリソース設定
	D3D12_RESOURCE_DESC texResDesc{};
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texResDesc.Width = textureWidth;//幅
	texResDesc.Height = textureHeight;//高さ
	texResDesc.DepthOrArraySize = 1;
	texResDesc.MipLevels = 1;
	texResDesc.SampleDesc.Count = 1;

	//テクスチャバッファ生成
	ComPtr<ID3D12Resource> texBuff;
	result = device_->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));

	//テクスチャバッファにデータ転送
	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Vector4) * textureWidth,//1ラインサイズ
		sizeof(Vector4) * imageDataCount//全サイズ
	);

	//元データ開放
	delete[] imageData;
}

void Sprite::GenerateDescriptorHeap(){
	HRESULT result;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定を元にSRV用デスクリプタヒープ生成
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	result = device_->CreateDescriptorHeap(
		&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}