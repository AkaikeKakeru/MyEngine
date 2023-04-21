#pragma once
#include <string>

//ノード構造体
struct Node {

};

//Fbxモデル
class FbxModel {
public:
	//フレンドクラス
	friend class FbxLoader;

private:
	//モデル名
	std::string name_;
};