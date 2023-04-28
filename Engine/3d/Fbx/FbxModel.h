#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>

#include <string>
#include "Vector3.h"
#include "Matrix4.h"
#include <vector>
#include <DirectXTex.h>

//ノード構造体
struct Node {
	//名前
	std::string name_;

	//ローカルスケール
	Vector3 scaling_ = { 1,1,1 };

	//ローカル回転
	Vector3 rotation_ = { 0,0,0 };

	//ローカル移動
	Vector3 translation_ = { 0,0,0 };

	//ローカル変形行列
	Matrix4 transform_;

	//グローバル変形行列
	Matrix4 globalTransform_;

	//親ノード
	Node* parent_ = nullptr;
};

//Fbxモデル
class FbxModel {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T> using vector = std::vector <T>;

public:
	//フレンドクラス
	friend class FbxLoader;

public://サブクラス
	//頂点データ構造体
	struct VertexPosNormalUv {
		Vector3 pos_;
		Vector3 normal_;
		Vector3 uv_;
	};

public://メンバ関数
	//バッファ生成
	void CreateBuffers(ID3D12Device* device);

private://メンバ変数
	///モデル関係

	//モデル名
	string name_;
	//ノード配列
	vector<Node> nodes_;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	vector<VertexPosNormalUv> vertices_;
	//頂点インデックス配列
	vector<unsigned short> indices_;

	///マテリアル関係

	//アンビエント係数
	Vector3 ambient_ = { 1,1,1 };
	//ディフューズ係数
	Vector3 diffuse_ = { 1,1,1 };
	//テクスチャメタデータ
	TexMetadata metadata = {};
	//スクラッチイメージ
	ScratchImage scratchImg_ = {};

	///バッファ関係

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	//SRVデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
};