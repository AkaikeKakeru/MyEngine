#pragma once

#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>

#include <string>

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

public: //定数
	//モデル格納ルートパス
	static const string BaseDirectory_;

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