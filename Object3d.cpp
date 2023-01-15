#include "Object3d.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Degree.h"
#include "WinApp.h"
#include "Model.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const float Object3d::radius = 5.0f;				// 底面の半径
const float Object3d::prizmHeight = 8.0f;			// 柱の高さ
ID3D12Device* Object3d::device = nullptr;
ID3D12GraphicsCommandList* Object3d::cmdList = nullptr;
ComPtr<ID3D12RootSignature> Object3d::rootsignature;
ComPtr<ID3D12PipelineState> Object3d::pipelinestate;
ComPtr<ID3D12DescriptorHeap> Object3d::descHeap;

ViewProjection Object3d::viewProjection_;

void Object3d::StaticInitialize(ID3D12Device* device, int window_width, int window_height) {
	// nullptrチェック
	assert(device);

	Object3d::device = device;
	Model::SetDevice(device);

	viewProjection_.Initialize();

	// カメラ初期化
	InitializeCamera(window_width, window_height);

	// パイプライン初期化
	InitializeGraphicsPipeline();
}

void Object3d::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Object3d::cmdList == nullptr);

	// コマンドリストをセット
	Object3d::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3d::PostDraw() {
	// コマンドリストを解除
	Object3d::cmdList = nullptr;
}

Object3d* Object3d::Create() {

	// 3Dオブジェクトのインスタンスを生成
	Object3d* object3d = new Object3d();
	if (object3d == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	//スケールのセット
	float scale_val = 5;
	object3d->worldTransform_.scale_ = { scale_val,scale_val,scale_val };

	return object3d;
}

void Object3d::SetEye(Vector3 eye) {
	Object3d::viewProjection_.camera_.eye_ = eye;

	UpdateViewMatrix();
}

void Object3d::SetTarget(Vector3 target) {
	Object3d::viewProjection_.camera_.target_ = target;

	UpdateViewMatrix();
}

void Object3d::CameraMoveVector(Vector3 move) {
	Vector3 eye_moved = GetEye();
	Vector3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetEye(eye_moved);
	SetTarget(target_moved);
}

void Object3d::CameraMoveEyeVector(Vector3 move) {
	Vector3 eye_moved = GetEye();
	Vector3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);
}

void Object3d::InitializeCamera(int window_width, int window_height) {
	//ビュー行列の計算
	UpdateViewMatrix();

	Vector4 pers = {
		1 / (static_cast<float>(tan(viewProjection_.angle_ / 2))) / viewProjection_.aspect_,
		1 / (static_cast<float>(tan(viewProjection_.angle_ / 2))),
		1 / (viewProjection_.farClip_ - viewProjection_.nearClip_) * viewProjection_.farClip_,
		-viewProjection_.nearClip_ / (viewProjection_.farClip_ - viewProjection_.nearClip_) * viewProjection_.farClip_,
	};

	viewProjection_.matProjection_ = Matrix4Identity();
	viewProjection_.matProjection_ = {
		pers.x,0,0,0,
		0,pers.y,0,0,
		0,0,pers.z,1,
		0,0,pers.w,0
	};
}

void Object3d::InitializeGraphicsPipeline() {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/Shader/ObjVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resource/Shader/ObjPS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[3] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootsignature.Get();

	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
	assert(SUCCEEDED(result));
}

void Object3d::UpdateViewMatrix() {
	//視点座標
	Vector3 eyePosition = viewProjection_.camera_.eye_;
	//注視点座標
	Vector3 targetPosition = viewProjection_.camera_.target_;
	//(仮の)上方向
	Vector3 upVector = viewProjection_.camera_.up_;

	Vector3 axisZ = Vector3Normalize(viewProjection_.camera_.target_ - viewProjection_.camera_.eye_);
	Vector3 axisX = Vector3Normalize(Vector3Cross(viewProjection_.camera_.up_, axisZ));
	Vector3 axisY = Vector3Cross(axisZ, axisX);

	//カメラ回転行列
	Matrix4 matCameraRot;
	//カメラ座標系→ワールド座標系の変換行列0
	matCameraRot = {
		axisX.x,axisX.y,axisX.z,0,
		axisY.x,axisY.y,axisY.z,0,
		axisZ.x,axisZ.y,axisZ.z,0,
		0,0,0,1
	};

	//転置により逆行列(逆回転)を計算
	viewProjection_.matView_ = Matrix4Transposed(matCameraRot);

	//視点座標に-1を掛けた座標
	Vector3 reverseEyePosition = eyePosition * -1;
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	Vector3 cameraMoveVal_ = {
		Vector3Dot(reverseEyePosition,axisX),
		Vector3Dot(reverseEyePosition,axisY),
		Vector3Dot(reverseEyePosition,axisZ)
	};

	//ビュー行列に平行移動成分を設定
	viewProjection_.matView_.m[3][0] = cameraMoveVal_.x;
	viewProjection_.matView_.m[3][1] = cameraMoveVal_.y;
	viewProjection_.matView_.m[3][2] = cameraMoveVal_.z;
}

bool Object3d::Initialize() {
	// nullptrチェック
	assert(device);

	worldTransform_.Initialize();

	//// ヒーププロパティ
	//CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);

	// リソース設定
	resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff);

	return true;
}

void Object3d::Update() {
	HRESULT result;

	worldTransform_.UpdateMatrix();

	// 定数バッファへデータ転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = worldTransform_.constBuff_->Map(0, nullptr, (void**)&constMap0);

	//constMap->color = color;

	// 行列の合成
	worldTransform_.constMap_->mat_ =
		worldTransform_.matWorld_
		* viewProjection_.matView_
		* viewProjection_.matProjection_;
	worldTransform_.constBuff_->Unmap(0, nullptr);
}

void Object3d::Draw() {
	// nullptrチェック
	assert(device);
	assert(Object3d::cmdList);

	if (model_ == nullptr) return;

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, worldTransform_.constBuff_->GetGPUVirtualAddress());

	model_->Draw(cmdList, 1);
}