#pragma once
#include "ObjectMesh.h"

#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <unordered_map>

class ObjectModel {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://静的メンバ関数
	   //静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//OBJ読み込み
	static ObjectModel* LoadFromOBJ(const std::string& modelname, bool smoothing = false);

private://メンバ関数
		// テクスチャ読み込み
	void LoadTextures();

	// マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// マテリアル登録
	void AddMaterial(ObjectMaterial* material) {
		// コンテナに登録
		materials_.emplace(material->name_, material);
	}

	void LoadFromOBJInternal(const std::string& modelname, bool smoothing = false);

	//デスクリプタヒープの初期化
	void InitializeDescriptorHeap();


public://メンバ関数
	   // デストラクタ
	~ObjectModel();

	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	//デバイス
	static ComPtr<ID3D12Device> device_;
	// デスクリプタサイズ
	static UINT descriptorIncrementSize_;
	//ディレクトリパス
	static const std::string Directory_;

private:
	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<ObjectMesh*> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, ObjectMaterial*> materials_;
	// デフォルトマテリアル
	ObjectMaterial* defaultMaterial_ = nullptr;

	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
};