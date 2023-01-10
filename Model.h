#pragma once
#include "Object3d.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

class Model {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体
	// 頂点データ構造体
	struct VertexPosNormalUv{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	//マテリアル
	struct Material{
		std::string name; //マテリアル名
		Vector3 ambient; //アンビエント影響度
		Vector3 diffuse; //ディフューズ影響度
		Vector3 specular; //スペキュラー影響度
		float alpha; //アルファ
		std::string textureFilename; //テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

public://静的関数
	Model* LoadFromOBJ();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <returns>成否</returns>
	bool LoadTexture(const std::string& directoryPath, const std::string filename);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath,const std::string& filename);

private: // 非公開メンバ変数
	void LoadFromOBJInternal();

private:
	//デバイス
	ComPtr<ID3D12Device> device_;

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;
	//マテリアル
	Material material_;
};