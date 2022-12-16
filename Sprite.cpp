#include "Sprite.h"
#include "SafeDelete.h"
#include "Degree.h"
#include "WinApp.h"
#include <DirectXTex.h>
#include <cassert>

using namespace DirectX;

void Sprite::Initialize(DrawBasis* drawBas) {
	assert(drawBas);
	drawBas_ = drawBas;
	device_ = drawBas_->GetDevice();
	cmdList_ = drawBas_->GetCommandList();

	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = ConvertToRadian(0.0f);
	worldTransform_.position = { 0,0 };
	worldTransform_.matWorld = Matrix4Identity();

	matOrtGrapricProjection_ = Matrix4Identity();
	/*テクスチャの左上を、画面の左上角に合わせたい*/
	//ポリゴンの左上を、画面中央に合わせる
	matOrtGrapricProjection_.m[0][0] = 2.0f / WinApp::Win_Width;
	matOrtGrapricProjection_.m[1][1] = -2.0f / WinApp::Win_Height;
	//上の状態から、画面半分くらいの距離だけ、左上にずらす
	matOrtGrapricProjection_.m[3][0] = -1.0f;
	matOrtGrapricProjection_.m[3][1] = 1.0f;

	CreateVertexBufferView();
	GenerateConstBuffer();
	GenerateTextureBuffer();
	GenerateDescriptorHeap();

	//SRVヒープの先頭アドレスを取得
	srvHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();

	CreateShaderResourceView();
}

void Sprite::Update() {
	float left = 0.0f;//左
	float right = size_.x;//右
	float top = 0.0f;//上
	float bottom = size_.y;//下

	//頂点データを設定
	vertices_[LeftBottom].pos = Vector3(left, bottom, 0);
	vertices_[LeftTop].pos = Vector3(left, top, 0);
	vertices_[RightBottom].pos = Vector3(right, bottom, 0);
	vertices_[RightTop].pos = Vector3(right, top, 0);

	///値を書き込むと自動的に転送される

	//頂点データをGPUに転送
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//色情報をGPUに転送
	constMapMaterial_->color = color_;

	//ワールド行列を再計算
	ReCalcMatWorld();

	//ワールド座標情報をGPUに転送
	//ワールド変換行列と、平行投影変換行列を掛ける
	constMapTransform_->mat = worldTransform_.matWorld *= matOrtGrapricProjection_;
}

void Sprite::Draw() {
	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	cmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

void Sprite::CreateVertexBufferView() {
	HRESULT result;
#pragma region 頂点データ
	float left = 0.0f;//左
	float right = 100.0f;//右
	float top = 0.0f;//上
	float bottom = 100.0f;//下

	float leftUv = 0.0f;//左
	float rightUv = 1.0f;//右
	float topUv = 0.0f;//上
	float bottomUv = 1.0f;//下

	//頂点データを設定
	vertices_[LeftBottom].pos = Vector3(left, bottom, 0);
	vertices_[LeftTop].pos = Vector3(left, top, 0);
	vertices_[RightBottom].pos = Vector3(right, bottom, 0);
	vertices_[RightTop].pos = Vector3(right, top, 0);

	vertices_[LeftBottom].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTop].uv = Vector2(leftUv, topUv);
	vertices_[RightBottom].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTop].uv = Vector2(rightUv, topUv);

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));
#pragma endregion

#pragma region 頂点バッファ設定
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES vbHeapProp{};
	vbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC vbResDesc{};
	vbResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vbResDesc.Width = sizeVB;
	vbResDesc.Height = 1;
	vbResDesc.DepthOrArraySize = 1;
	vbResDesc.MipLevels = 1;
	vbResDesc.SampleDesc.Count = 1;
	vbResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
#pragma endregion

#pragma region 頂点バッファ生成
	//頂点バッファの生成
	result = device_->CreateCommittedResource(
		&vbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&vbResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 頂点バッファへ転送
	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));
	//全頂点に対して
	//座標をコピー
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);
#pragma endregion

#pragma region 頂点バッファビュー作成
	//頂点バッファビューの作成

	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion
}

void Sprite::GenerateConstBuffer() {
	GenerateConstMaterial();
	GenerateConstTransform();
}

void Sprite::GenerateConstMaterial() {
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

void Sprite::GenerateConstTransform() {
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

void Sprite::GenerateTextureBuffer() {
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resource/texture.png",//「Resources」フォルダの「texture.png」
		WIC_FLAGS_NONE,
		&metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT,
		0,
		mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	//テクスチャバッファヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//テクスチャバッファリソース設定
	texResDesc_.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc_.Format = metadata.format;
	texResDesc_.Width = metadata.width;//幅
	texResDesc_.Height = (UINT)metadata.height;//高さ
	texResDesc_.DepthOrArraySize = (UINT16)metadata.arraySize;
	texResDesc_.MipLevels = (UINT16)metadata.mipLevels;
	texResDesc_.SampleDesc.Count = 1;

	//テクスチャバッファ生成
	result = device_->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff_));

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		//テクスチャバッファにデータ転送
		result = texBuff_->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//全サイズ
		);
		assert(SUCCEEDED(result));
	}
}

void Sprite::GenerateDescriptorHeap() {
	HRESULT result;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = static_cast<UINT>(kMaxSRVCount);

	//設定を元にSRV用デスクリプタヒープ生成
	result = device_->CreateDescriptorHeap(
		&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));
}

void Sprite::CreateShaderResourceView() {
	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = texResDesc_.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = texResDesc_.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device_->CreateShaderResourceView(texBuff_.Get(), &srvDesc, srvHandle_);
}

void Sprite::ReCalcMatWorld() {
	worldTransform_.matWorld = Matrix4Identity();

	worldTransform_.matWorld *=
		Matrix4RotationZ(worldTransform_.rotation);
	worldTransform_.matWorld *=
		Matrix4Translation(
			Vector3(
				worldTransform_.position.x,
				worldTransform_.position.y,
				0.0f));
}