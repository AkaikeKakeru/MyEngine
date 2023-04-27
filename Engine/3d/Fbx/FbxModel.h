#pragma once
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

private:
	//モデル名
	std::string name_;
	//ノード配列
	std::vector<Node> nodes_;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	//頂点インデックス配列
	std::vector<unsigned short> indices_;

	//アンビエント係数
	Vector3 ambient_ = { 1,1,1 };
	//ディフューズ係数
	Vector3 diffuse_ = { 1,1,1 };
	//テクスチャメタデータ
	DirectX::TexMetadata metadata = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg_ = {};
};