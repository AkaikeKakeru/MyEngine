#include "Model.h"
#include "SafeDelete.h"
#include "Degree.h"
#include "WinApp.h"
#include <cassert>

void Model::Initialize(ObjectBasis* objBas) {
	assert(objBas);
	objBas_ = objBas;
	device_ = objBas_->GetDevice();
	cmdList_ = objBas_->GetCommandList();

	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = {
		ConvertToRadian(0.0f),
		ConvertToRadian(0.0f),
		ConvertToRadian(0.0f)};
	worldTransform_.position = { 0,0,0 };
	worldTransform_.matWorld = Matrix4Identity();

	CreateVertexBufferView();
	CreateIndexBufferView();
	GenerateConstBuffer();
}

void Model::Update() {
	//上下左右の数値の更新
	dir_.left = 0.0f * size_.x;
	dir_.right = 1.0f * size_.x;
	dir_.top = 0.0f * size_.y;
	dir_.bottom = 1.0f * size_.y;

	//頂点データを設定
	vertices_[LeftBottom].pos = Vector3(dir_.left, dir_.bottom, 0);
	vertices_[LeftTop].pos = Vector3(dir_.left, dir_.top, 0);
	vertices_[RightBottom].pos = Vector3(dir_.right, dir_.bottom, 0);
	vertices_[RightTop].pos = Vector3(dir_.right, dir_.top, 0);

	///値を書き込むと自動的に転送される

	//頂点データをGPUに転送
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//色情報をGPUに転送
	constMapMaterial_->color = color_;

	//ワールド行列を再計算
	ReCalcMatWorld();

	//ワールド座標情報をGPUに転送
	//ワールド変換行列と、平行投影変換行列を掛ける
	constMapTransform_->mat = worldTransform_.matWorld;
}

void Model::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	//描画用テクスチャコマンド
	objBas_->SetTextureCommand(textureIndex_);

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

void Model::CreateVertexBufferView() {
	HRESULT result;
#pragma region 頂点データ
	//上下左右の数値の設定
	dir_.left = 0.0f * size_.x;
	dir_.right = 1.0f * size_.x;
	dir_.top = 0.0f * size_.y;
	dir_.bottom = 1.0f * size_.y;

	float leftUv = 0.0f;//左
	float rightUv = 1.0f;//右
	float topUv = 0.0f;//上
	float bottomUv = 1.0f;//下

	//頂点データを設定
	vertices_[LeftBottom].pos = Vector3(dir_.left, dir_.bottom, 0);
	vertices_[LeftTop].pos = Vector3(dir_.left, dir_.top, 0);
	vertices_[RightBottom].pos = Vector3(dir_.right, dir_.bottom, 0);
	vertices_[RightTop].pos = Vector3(dir_.right, dir_.top, 0);

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

void Model::CreateIndexBufferView() {
	HRESULT result;
#pragma region インデックスデータ
	uint16_t indicesOrigin[] = {
		0,1,2,//三角形1つ目
		1,2,3,//三角形2つ目
	};

	for (size_t i = 0; i < kIndicesNum; i++){
		indices_[i] = indicesOrigin[i];
	}
	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices_));
#pragma endregion

#pragma region インデックスバッファ設定
	//リソース設定
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES ibHeapProp{};
	ibHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC ibResDesc{};
	ibResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ibResDesc.Width = sizeIB;
	ibResDesc.Height = 1;
	ibResDesc.DepthOrArraySize = 1;
	ibResDesc.MipLevels = 1;
	ibResDesc.SampleDesc.Count = 1;
	ibResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
#pragma endregion

#pragma region インデックスバッファ生成
	//インデックスバッファの生成
	result = device_->CreateCommittedResource(
		&ibHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&ibResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indBuff_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region インデックスバッファへ転送
	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	result = indBuff_->Map(0, nullptr, (void**)&indMap_);
	assert(SUCCEEDED(result));
	//全インデックスに対して
	//インデックスをコピー
	std::copy(std::begin(indices_), std::end(indices_), indMap_);

	//繋がりを解除
	indBuff_->Unmap(0, nullptr);
#pragma endregion

#pragma region インデックスバッファビュー作成
	//インデックスバッファビューの作成

	//GPU仮想アドレス
	ibView_.BufferLocation = indBuff_->GetGPUVirtualAddress();
	//フォーマット
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	//インデックスバッファのサイズ
	ibView_.SizeInBytes = sizeIB;
#pragma endregion
}

void Model::GenerateConstBuffer() {
	GenerateConstMaterial();
	GenerateConstTransform();
}

void Model::GenerateConstMaterial() {
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

void Model::GenerateConstTransform() {
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
	constMapTransform_->mat = worldTransform_.matWorld;
}

void Model::ReCalcMatWorld() {
	worldTransform_.matWorld = Matrix4Identity();

	worldTransform_.matWorld *=
		Matrix4Scale(worldTransform_.scale);
	worldTransform_.matWorld *=
		Matrix4Rotation(worldTransform_.rotation);
	worldTransform_.matWorld *=
		Matrix4Translation(worldTransform_.position);
}