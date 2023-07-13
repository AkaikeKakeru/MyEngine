﻿#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>

#include <string>
#include "Vector3.h"
#include "Matrix4.h"
#include <vector>
#include <DirectXTex.h>

#include <fbxsdk.h>

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

public://定数
	//ボーンインデックスの最大値
	static const int MAX_BONE_INDICES_ = 4;

public://サブクラス
	//頂点データ構造体
	struct VertexPosNormalUvSkin {
		Vector3 pos_;
		Vector3 normal_;
		Vector3 uv_;
		UINT boneIndex_[MAX_BONE_INDICES_]; //ボーン 番号
		float boneWeight_[MAX_BONE_INDICES_]; //ボーン 重み
	};

	//ボーン構造体
	struct Bone {
		//名前
		std::string name_;

		//初期姿勢の逆行列
		Matrix4 invInitialPose_;

		//クラスター(FBX側のボーン情報)
		FbxCluster* fbxCluster_;

		//コンストラクタ
		Bone(const std::string& name) {
			this->name_ = name;
		}
	};

public://メンバ関数
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	   
	//バッファ生成
	void CreateBuffers(ID3D12Device* device);

public://アクセッサ
	//モデルの変形行列取得
	const Matrix4& GetModelTransform() {
		return meshNode->globalTransform_;
	}

	//ボーン配列の取得
	std::vector<Bone>& GetBones() {
		return bones_;
	}
	 
	//FBXシーンの取得
	FbxScene* GetFbxScene() {
		return fbxScene_;
	}

private://メンバ変数
	//FBXシーン
	FbxScene* fbxScene_ = nullptr;

	///ボーン関係

	//ボーン配列
	std::vector<Bone> bones_;

	///モデル関係

	//モデル名
	string name_;
	//ノード配列
	vector<Node> nodes_;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	vector<VertexPosNormalUvSkin> vertices_;
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
	//マテリアルバッファ
	ComPtr<ID3D12Resource> materialBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	//SRVデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

public: //デストラクタ
	~FbxModel();
};