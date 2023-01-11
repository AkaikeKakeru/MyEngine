#include "TextureManager.h"
#include <DirectXTex.h>
using namespace DirectX;

uint32_t TextureManager::Load(const std::string& filename){
	return TextureManager::GetInstance()->LoadInternal(filename);
}

void TextureManager::Initialize(ID3D12Device* device, std::string directoryPath){
	assert(device);

	device_ = device;
	textureDhirectoryPath_ = directoryPath;

	incrementSize_ =
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	GenerateDescriptorHeap();

	ResetAll();
}

void TextureManager::GenerateDescriptorHeap() {
	HRESULT result = S_FALSE;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = static_cast<UINT>(kDescriptors);

	//設定を元にSRV用デスクリプタヒープ生成
	result = device_->CreateDescriptorHeap(
		&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));
}

void TextureManager::ResetAll(){
	indexNextDescriptorHeap_ = 0;

	// 全テクスチャを初期化
	for (size_t i = 0; i < kDescriptors; i++) {
		textures_[i].resource_.Reset();
		textures_[i].srvCpuHandle_.ptr = 0;
		textures_[i].srvGpuHandle_.ptr = 0;
		textures_[i].name.clear();
	}
}

const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(uint32_t textureHandle) {
	assert(textureHandle < textures_.size());
	Texture& texture = textures_.at(textureHandle);
	return texture.resource_->GetDesc();
}

void TextureManager::SetGraphicsRootDescriptorTable(
	ID3D12GraphicsCommandList* commandList, 
	UINT rootParamIndex,
	uint32_t textureHandle) { 
	// デスクリプタヒープの配列
	assert(textureHandle < textures_.size());
	ID3D12DescriptorHeap* ppHeaps[] = {srvHeap_.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// シェーダリソースビューをセット
	commandList->SetGraphicsRootDescriptorTable(
		rootParamIndex, textures_[textureHandle].srvGpuHandle_);
}

uint32_t TextureManager::LoadInternal(const std::string& filename){
	assert(indexNextDescriptorHeap_ < kDescriptors);
	uint32_t handle = indexNextDescriptorHeap_;

	// 読み込み済みテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
		return texture.name == filename;
		});
	if (it != textures_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}
	
	LoadTexture(handle,filename);

	indexNextDescriptorHeap_++;

	return handle;
}

void TextureManager::LoadTexture(uint32_t textureHandle, const std::string& filename) {
	// 書き込むテクスチャの参照
	Texture& texture = textures_.at(textureHandle);
	texture.name = filename;
	
	//ディレクトリパスとファイル名を連結して、フルパスを得る
	std::string fullPath = textureDhirectoryPath_ + filename;//「Resources」+「○○.拡張子」

	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(
		CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	//ワイド文字列に変換
	std::vector<wchar_t> wfilePath(filePathBufferSize);
	MultiByteToWideChar(
		CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

	GenerateTextureBuffer(texture,wfilePath.data());
	////SRVヒープの先頭アドレスを取得
	//texture.srvCpuHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();
	//texture.srvGpuHandle_ = srvHeap_->GetGPUDescriptorHandleForHeapStart();

	//for (size_t i = 0; i < textureHandle; i++){
	//	texture.srvCpuHandle_.ptr += incrementSize_;
	//	texture.srvGpuHandle_.ptr += incrementSize_;
	//}
	CreateShaderResourceView(texture,textureHandle);
}

void TextureManager::GenerateTextureBuffer(Texture texture,const wchar_t* wfilename) {
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		wfilename,
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
		IID_PPV_ARGS(&textures_[0].resource_));
	assert(SUCCEEDED(result));

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		//テクスチャバッファにデータ転送
		result = textures_[0].resource_->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//全サイズ
		);
		assert(SUCCEEDED(result));
	}
}

void TextureManager::CreateShaderResourceView(Texture texture,uint32_t textureHandle) {

	texture.srvCpuHandle_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		srvHeap_->GetCPUDescriptorHandleForHeapStart(), textureHandle, incrementSize_);
	texture.srvGpuHandle_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		srvHeap_->GetGPUDescriptorHandleForHeapStart(), textureHandle, incrementSize_);

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	texResDesc_ = texture.resource_->GetDesc();

	srvDesc.Format = texResDesc_.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = texResDesc_.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device_->CreateShaderResourceView(
		texture.resource_.Get(),
		&srvDesc, 
		texture.srvCpuHandle_);
}

TextureManager* TextureManager::GetInstance(){
	static TextureManager instance;
	return &instance;
}
