#include "ModelBackup.h"
#include "SafeDelete.h"
#include "Degree.h"
#include "WinApp.h"
#include <cassert>

ModelBackup::ViewProjection ModelBackup::viewProjection_;

void ModelBackup::Initialize(ObjectBasis* objBas) {
	assert(objBas);
	objBas_ = objBas;
	device_ = objBas_->GetDevice();
	cmdList_ = objBas_->GetCommandList();

	InitWorldTransform();
	InitProjection();
	InitView();

	CreateVertexBufferView();
	CreateIndexBufferView();
	GenerateConstBuffer();
}

void ModelBackup::Update() {
	CreateVertexBufferView();
	CreateIndexBufferView();
	GenerateConstBuffer();

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

void ModelBackup::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	//描画用テクスチャコマンド
	objBas_->SetTextureCommand(textureIndex_);

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//インデックスバッファビューの設定コマンド
	cmdList_->IASetIndexBuffer(&ibView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawIndexedInstanced(_countof(indices_), 1, 0, 0, 0);
}

void ModelBackup::InitWorldTransform() {
	worldTransform_.scale = { 1,1,0 };
	worldTransform_.rotation = {
		ConvertToRadian(0.0f),
		ConvertToRadian(0.0f),
		ConvertToRadian(0.0f) };
	worldTransform_.position = { 0,0,0 };
	worldTransform_.matWorld = Matrix4Identity();
}

void ModelBackup::InitView() {
	viewProjection_.eye = { 0,0,-100 };
	viewProjection_.target = { 0,0,0 };
	viewProjection_.up = { 0,1,0 };

	Vector3 axisZ_ = Vector3Normalize(viewProjection_.target - viewProjection_.eye);
	Vector3 axisX_ = Vector3Normalize(Vector3Cross(viewProjection_.up, axisZ_));
	Vector3 axisY_ = Vector3Cross(axisZ_, axisX_);

	Vector3 cameraMoveVal_ = {
		Vector3Dot(viewProjection_.eye,axisX_),
		Vector3Dot(viewProjection_.eye,axisY_),
		Vector3Dot(viewProjection_.eye,axisZ_)
	};

	viewProjection_.matView = Matrix4Identity();
	viewProjection_.matView = {
		axisX_.x,axisX_.y,axisX_.z,0,
		axisY_.x,axisY_.y,axisY_.z,0,
		axisZ_.x,axisZ_.y,axisZ_.z,0,
		-cameraMoveVal_.x,-cameraMoveVal_.y,-cameraMoveVal_.z,1
	};
}

void ModelBackup::InitProjection() {
	viewProjection_.angle = ConvertToRadian(45.0f);
	viewProjection_.aspect = (float)WinApp::Win_Width / WinApp::Win_Height;
	viewProjection_.nearClip = 0.1f;
	viewProjection_.farClip = 1000.0f;

	Vector4 pers = {
		1 / (static_cast<float>(tan(viewProjection_.angle / 2))) / viewProjection_.aspect,
		1 / (static_cast<float>(tan(viewProjection_.angle / 2))),
		1 / (viewProjection_.farClip - viewProjection_.nearClip) * viewProjection_.farClip,
		-viewProjection_.nearClip / (viewProjection_.farClip - viewProjection_.nearClip) * viewProjection_.farClip,
	};

	viewProjection_.matPerspective = Matrix4Identity();
	viewProjection_.matPerspective = {
		pers.x,0,0,0,
		0,pers.y,0,0,
		0,0,pers.z,1,
		0,0,pers.w,0
	};
}

void ModelBackup::CreateVertexBufferView() {
	HRESULT result;
#pragma region 頂点データ
	//上下左右の数値の設定
	dir_.left = -0.4f;// *size_.x;
	dir_.right = +0.4f;// * size_.x;
	dir_.top = +0.7f;// * size_.y;
	dir_.bottom = -0.7f;// * size_.y;
	dir_.front = -50.0f;
	dir_.back = 50.0f;

	float leftUv = 0.0f;//左
	float rightUv = 1.0f;//右
	float topUv = 0.0f;//上
	float bottomUv = 1.0f;//下

						  //頂点データを設定
						  //前
	vertices_[LeftBottomFront].pos = Vector3(dir_.left, dir_.bottom, dir_.front);
	vertices_[LeftTopFront].pos = Vector3(dir_.left, dir_.top, dir_.front);
	vertices_[RightBottomFront].pos = Vector3(dir_.right, dir_.bottom, dir_.front);
	vertices_[RightTopFront].pos = Vector3(dir_.right, dir_.top, dir_.front);

	vertices_[LeftBottomFront].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTopFront].uv = Vector2(leftUv, topUv);
	vertices_[RightBottomFront].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTopFront].uv = Vector2(rightUv, topUv);

	//後ろ
	vertices_[LeftBottomBack].pos = Vector3(dir_.left, dir_.bottom, dir_.back);
	vertices_[LeftTopBack].pos = Vector3(dir_.left, dir_.top, dir_.back);
	vertices_[RightBottomBack].pos = Vector3(dir_.right, dir_.bottom, dir_.back);
	vertices_[RightTopBack].pos = Vector3(dir_.right, dir_.top, dir_.back);

	vertices_[LeftBottomBack].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTopBack].uv = Vector2(leftUv, topUv);
	vertices_[RightBottomBack].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTopBack].uv = Vector2(rightUv, topUv);

	//左
	vertices_[LeftBottomLeft].pos = Vector3(dir_.left, dir_.bottom, dir_.front);
	vertices_[LeftTopLeft].pos = Vector3(dir_.left, dir_.bottom, dir_.back);
	vertices_[RightBottomLeft].pos = Vector3(dir_.left, dir_.top, dir_.front);
	vertices_[RightTopLeft].pos = Vector3(dir_.left, dir_.top, dir_.back);

	vertices_[LeftBottomLeft].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTopLeft].uv = Vector2(leftUv, topUv);
	vertices_[RightBottomLeft].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTopLeft].uv = Vector2(rightUv, topUv);

	//右
	vertices_[LeftBottomRight].pos = Vector3(dir_.right, dir_.bottom, dir_.front);
	vertices_[LeftTopRight].pos = Vector3(dir_.right, dir_.bottom, dir_.back);
	vertices_[RightBottomRight].pos = Vector3(dir_.right, dir_.top, dir_.front);
	vertices_[RightTopRight].pos = Vector3(dir_.right, dir_.top, dir_.back);

	vertices_[LeftBottomRight].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTopRight].uv = Vector2(leftUv, topUv);
	vertices_[RightBottomRight].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTopRight].uv = Vector2(rightUv, topUv);

	//下
	vertices_[LeftBottomBottom].pos = Vector3(dir_.left, dir_.bottom, dir_.front);
	vertices_[LeftTopBottom].pos = Vector3(dir_.left, dir_.bottom, dir_.back);
	vertices_[RightBottomBottom].pos = Vector3(dir_.right, dir_.bottom, dir_.front);
	vertices_[RightTopBottom].pos = Vector3(dir_.right, dir_.bottom, dir_.back);

	vertices_[LeftBottomBottom].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTopBottom].uv = Vector2(leftUv, topUv);
	vertices_[RightBottomBottom].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTopBottom].uv = Vector2(rightUv, topUv);

	//上
	vertices_[LeftBottomTop].pos = Vector3(dir_.left, dir_.top, dir_.front);
	vertices_[LeftTopTop].pos = Vector3(dir_.left, dir_.top, dir_.back);
	vertices_[RightBottomTop].pos = Vector3(dir_.right, dir_.top, dir_.front);
	vertices_[RightTopTop].pos = Vector3(dir_.right, dir_.top, dir_.back);

	vertices_[LeftBottomTop].uv = Vector2(leftUv, bottomUv);
	vertices_[LeftTopTop].uv = Vector2(leftUv, topUv);
	vertices_[RightBottomTop].uv = Vector2(rightUv, bottomUv);
	vertices_[RightTopTop].uv = Vector2(rightUv, topUv);

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

void ModelBackup::CreateIndexBufferView() {
	HRESULT result;
#pragma region インデックスデータ
	unsigned short indicesOrigin[] = {
		//前
		0,1,2,//三角形1つ目
		1,2,3,//三角形2つ目
			  //後ろ
			  4,5,6,//三角形3つ目
			  5,6,7,//三角形4つ目
					//左
					8,9,10,//三角形5つ目
					9,10,11,//三角形6つ目
							//右
							12,13,14,//三角形7つ目
							13,14,15,//三角形8つ目
									 //下
									 16,17,18,//三角形9つ目
									 17,18,19,//三角形10つ目
											  //上
											  20,21,22,//三角形11つ目
											  21,22,23,//三角形12つ目
	};

	for (size_t i = 0; i < _countof(indices_); i++) {
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

void ModelBackup::GenerateConstBuffer() {
	GenerateConstMaterial();
	GenerateConstTransform();
}

void ModelBackup::GenerateConstMaterial() {
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

void ModelBackup::GenerateConstTransform() {
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

	//ワールド変換行列と、ビュー行列と、透視投影変換行列を掛ける
	constMapTransform_->mat = worldTransform_.matWorld *=
		viewProjection_.matView *=
		viewProjection_.matPerspective;
}

void ModelBackup::ReCalcMatWorld() {
	worldTransform_.matWorld = Matrix4Identity();

	worldTransform_.matWorld *=
		Matrix4Scale(worldTransform_.scale);
	worldTransform_.matWorld *=
		Matrix4Rotation(worldTransform_.rotation);
	worldTransform_.matWorld *=
		Matrix4Translation(worldTransform_.position);
}

void ModelBackup::ReCalcMatView()
{
	Vector3 axisZ_ = Vector3Normalize(viewProjection_.target - viewProjection_.eye);
	Vector3 axisX_ = Vector3Normalize(Vector3Cross(viewProjection_.up, axisZ_));
	Vector3 axisY_ = Vector3Cross(axisZ_, axisX_);

	Vector3 cameraMoveVal_ = {
		Vector3Dot(viewProjection_.eye,axisX_),
		Vector3Dot(viewProjection_.eye,axisY_),
		Vector3Dot(viewProjection_.eye,axisZ_)
	};

	viewProjection_.matView = Matrix4Identity();
	viewProjection_.matView = {
		axisX_.x,axisX_.y,axisX_.z,0,
		axisY_.x,axisY_.y,axisY_.z,0,
		axisZ_.x,axisZ_.y,axisZ_.z,0,
		-cameraMoveVal_.x,-cameraMoveVal_.y,-cameraMoveVal_.z,1
	};
}

void ModelBackup::CameraMoveEye(Vector3 move){
	Vector3 eye_moved = GetEye();
	Vector3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);
}