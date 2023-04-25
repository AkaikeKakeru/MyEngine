#pragma once
#include <string>
#include "Vector3.h"
#include "Matrix4.h"

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

private:
	//モデル名
	std::string name_;
	//ノード配列
	std::vector<Node> nodes_;
};