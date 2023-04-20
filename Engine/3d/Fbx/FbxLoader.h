#pragma once

#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>

class FbxLoader {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

public: //メンバ関数
	//初期化
	void Initialize(ID3D12Device* device);

	//後始末
	void Finalize();

private: //メンバ変数
	//デバイス
	ComPtr<ID3D12Device> device_ = nullptr;

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