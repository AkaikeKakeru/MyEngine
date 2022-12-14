#include "DrawBasis.h"
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

void DrawBasis::Initialize() {
	dxBas_ = DirectXBasis::GetInstance();

	device_ = dxBas_->GetDevice();
	cmdList_ = dxBas_->GetCommandList();

	CreateVertexBufferView();
	CompileShaderFile();
	AssembleVertexLayout();
	CreateGraphicsPipeline();
}

void DrawBasis::Draw(){
	//パイプラインステートとルートシグネイチャの設定コマンド
	cmdList_->SetPipelineState(pipelineState_.Get());
	cmdList_->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

void DrawBasis::CreateVertexBufferView() {
	HRESULT result;
#pragma region 頂点データ
	//頂点データ
	Vector3 vert[] = {
		{-0.5f,0.5f,0.0f},//左下
		{-0.5f,+0.5f,0.0f},//左上
		{+0.5f,-0.5f,0.0f},//右下
	};
	
	//頂点部位
	typedef enum VerticesParts {
		LeftBottom,//左下
		LeftTop,//左上
		RightBottom,//右下
	}VerticesParts;

	float left = -0.5f;//左
	float right = +0.5f;//右
	float top = +0.5f;//上
	float bottom = -0.5f;//下

	VertexPos vertices[kVerticesNum]{};

	//頂点データを設定
	vertices[LeftBottom].pos = Vector3(left, bottom, 0);
	vertices[LeftTop].pos = Vector3(left, top, 0);
	vertices[RightBottom].pos = Vector3(right, bottom, 0);

	//頂点データ

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));
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
	VertexPos* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		//座標をコピー
		vertMap[i] = vertices[i];
	}
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
	vbView_.StrideInBytes = sizeof(Vector3);
#pragma endregion
}

void DrawBasis::CompileShaderFile() {
	HRESULT result;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/SpriteVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダ―モデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&vsBlob_, &errorBlob_);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからのエラー内容をコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/shader/SpritePS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダ―モデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob_, &errorBlob_);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからのエラー内容をコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void DrawBasis::AssembleVertexLayout() {
	//頂点レイアウト
	enum LayoutElement {
		Position,
	};

	inputLayout_[Position] = {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
	};
}

void DrawBasis::CreateGraphicsPipeline() {
	AssembleGraphicsPipeline();
	GenerateRootSignature();
	GeneratePipelineState();
}

void DrawBasis::AssembleGraphicsPipeline() {
	//グラフィックスパイプライン設定

#pragma region シェーダ情報を組み込む
	//シェーダの設定
	pipelineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();
#pragma endregion

#pragma region サンプルマスクとラスタライザステート
	//サンプルマスクの設定
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザの設定
	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc_.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に
#pragma endregion

#pragma endregion ブレンドステート
	//ブレンドステート
	//pipelineDesc_.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てにチャネルを描画

	//レンダ―ターゲットビューのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc_.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てにチャネルを描画
#pragma endregion

#pragma region 頂点レイアウトの設定
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout_;
	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout_);
#pragma endregion 

#pragma region 図形の形状設定
	//三角形に設定
	pipelineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
#pragma endregion

#pragma region その他の設定
	pipelineDesc_.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
#pragma region
}
void DrawBasis::GenerateRootSignature() {
	HRESULT result;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSigetureDesc{};
	rootSigetureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;

	result = D3D12SerializeRootSignature(
		&rootSigetureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	result = device_->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
	//パイプラインにルートシグネチャをセット
	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void DrawBasis::GeneratePipelineState() {
	HRESULT result;
	//パイプラインステートの生成
	result = device_->CreateGraphicsPipelineState(
		&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

DrawBasis* DrawBasis::GetInstance(){
	static DrawBasis instance;
	return &instance;
}