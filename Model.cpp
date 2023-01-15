#include "Model.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <DirectXTex.h>
using namespace std;
using namespace DirectX;

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//デバイス
ComPtr<ID3D12Device> Model::device_ = nullptr;
// デスクリプタサイズ
UINT Model::descriptorIncrementSize_;

Model* Model::LoadFromOBJ(const std::string& modelname) {
	//インスタンス
	Model* model = new Model();

	//デスクリプタヒープの生成
	model->InitializeDescriptorHeap();

	//読み込み
	model->LoadFromOBJInternal(modelname);

	//バッファ生成
	model->CreateBaffers();

	return model;
}

void Model::LoadFromOBJInternal(const std::string& modelname) {
	//ファイルストリーム
	std::ifstream file;
	//モデル名
	const string filename = modelname + ".obj"; // "modelname.obj"
	const string directoryPath = "Resource/" + modelname + "/"; // "Resources/modelname/"

	//objファイルを開く
	file.open(directoryPath + filename);

	//ファイルオープンの失敗を確認
	assert(!file.fail());

	vector<Vector3>positions;//頂点座標
	vector<Vector3>normals;//法線ベクトル
	vector<Vector2>texcoords;//テクスチャUV
	//1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭文字列がvなら頂点座標
		if (key == "v") {
			//X,Y,Z座標読み込み
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//座標データに追加
			positions.emplace_back(position);
		}
		//先頭文字列がvtならテクスチャ
		if (key == "vt") {
			//U,V成分読み込み
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		//先頭文字列がvnなら法線ベクトル
		if (key == "vn") {
			//X,Y,Z座標読み込み
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		//先頭文字列がfならポリゴン(三角形)
		if (key == "f") {
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				//頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
				index_stream >> indexNormal;
				//頂点データの追加
				VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices_.emplace_back(vertex);
				//頂点インデックスに追加
				indices_.emplace_back((unsigned short)indices_.size());
			}
		}
		//先頭文字列がmtlliibならマテリアル
		if (key == "mtllib") {
			//マテリアルのファイル名読み込み
			string filename;
			line_stream >> filename;
			//マテリアル読み込み
			LoadMaterial(directoryPath, filename);
		}
	}
	//ファイルを閉じる
	file.close();
}

void Model::LoadTexture(const std::string& directoryPath, const std::string filename) {
	HRESULT result = S_FALSE;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	//ファイルパスを結合
	string filepath = directoryPath + filename;

	//ユニコード文字列に変換する
	wchar_t wfilepath[128];
	int iBufferSize = MultiByteToWideChar(
		CP_ACP, 0,
		filepath.c_str(), -1, wfilepath,
		_countof(wfilepath));

	result = LoadFromWICFile(
		wfilepath,
		WIC_FLAGS_NONE,
		&metadata,
		scratchImg);

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height, (UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr, IID_PPV_ARGS(&texbuff_));
	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); // 生データ抽出
		result = texbuff_->WriteToSubresource(
			(UINT)i,
			nullptr,              // 全領域へコピー
			img->pixels,          // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	// シェーダリソースビュー作成
	cpuDescHandleSRV_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), 0, descriptorIncrementSize_);
	gpuDescHandleSRV_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap_->GetGPUDescriptorHandleForHeapStart(), 0, descriptorIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff_->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device_->CreateShaderResourceView(texbuff_.Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		cpuDescHandleSRV_
	);
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename) {
	//ファイルストリーム
	std::ifstream file;
	//マテリアルファイルを開く
	file.open(directoryPath + filename);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		//1行分の文字列をストリームに変換
		std::istringstream line_stream(line);
		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin()); //先頭の文字を削除
		}

		//先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			//マテリアル名読み込み
			line_stream >> material_.name;
		}

		//先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material_.ambient.x;
			line_stream >> material_.ambient.y;
			line_stream >> material_.ambient.z;
		}
		//先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material_.diffuse.x;
			line_stream >> material_.diffuse.y;
			line_stream >> material_.diffuse.z;
		}
		//先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material_.specular.x;
			line_stream >> material_.specular.y;
			line_stream >> material_.specular.z;
		}

		//先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			//テクスチャのファイル名読み込み
			line_stream >> material_.textureFilename;
			//テクスチャ読み込み
			LoadTexture(directoryPath, material_.textureFilename);
		}
	}
}

void Model::InitializeDescriptorHeap() {
	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = 1; // シェーダーリソースビュー1つ
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	descriptorIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Model::CreateBaffers() {
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());

	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff_));

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapPropsMaterial = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDescMaterial = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapPropsMaterial, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&resourceDescMaterial,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	ConstBufferDataMaterial* constMap1 = nullptr;
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMap1);
	if (SUCCEEDED(result)) {
		constMap1->ambient = material_.ambient;
		constMap1->diffuse = material_.diffuse;
		constMap1->specular = material_.specular;
		constMap1->alpha = material_.alpha;
		constBuffMaterial_->Unmap(0, nullptr);
	}
}

void Model::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial) {
	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView_);
	// 定数バッファビューマテリアルをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial,
		constBuffMaterial_->GetGPUVirtualAddress());

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	if (material_.textureFilename.size() > 0) {
		// シェーダリソースビューをセット
		cmdList->SetGraphicsRootDescriptorTable(2, gpuDescHandleSRV_);
	}
	// 描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}