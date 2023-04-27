#pragma once

#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>

#include <string>

#include "FbxModel.h"

class FbxLoader {
private: //省略
	using string = std::string;

public: //静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

public: //基本メンバ関数
	//初期化
	void Initialize(ID3D12Device* device);

	//後始末
	void Finalize();

public: //固有メンバ関数
	//ファイルからFBXファイル読み込み
	void LoadModelFromFile(const string& modelName);

	//再帰的ノード構成を解析
	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);

	//メッシュを読み取り
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);

	//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);

private: //サブ関数
	///ParseMesh
	//頂点座標読み取り
	void ParseMeshVertices(FbxModel* Model, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	//マテリアル読み取り
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	//テクスチャ読み込み
	void LoadTexture(FbxModel* model, const std::string& fullPath);

private: //静的メンバ変数
	//モデル格納ルートパス
	static const string baseDirectory_;
	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName_;

private: //メンバ変数
	//デバイス
	ID3D12Device* device_ = nullptr;

	//マネージャー
	FbxManager* fbxManager_ = nullptr;

	//インポート
	FbxImporter* fbxImporter_ = nullptr;

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;
};