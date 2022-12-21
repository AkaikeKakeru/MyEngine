#pragma once
#include "ObjectBasis.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

class Model {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体
		//定数バッファ用データ構造体(マテリアル)
	typedef struct ConstBufferDataMaterial {
		Vector4 color;//色(RGBA)
	}ConstBufferDataMaterial;

	//定数バッファ用データ構造体(3D変換行列)
	typedef struct ConstBufferDataTransform {
		Matrix4 mat;//3D変換行列
	}ConstBufferDataTransform;

	//ワールド変換構造体
	typedef struct WorldTransform {
		Vector3 scale;		//スケール
		Vector3 rotation;	//回転
		Vector3 position;//平行移動
		Matrix4 matWorld = Matrix4Identity();	//ワールド変換行列
	}WorldTransform;

	//頂点データ構造体
	typedef struct VertexPosNormalUv {
		Vector3 pos;//xyz座標
		Vector2 normal;//法線
		Vector2 uv;//uv座標
	}Vertex;

	typedef struct TopBottomLeftRight {
		float left;//左
		float right;//右
		float top;//上
		float bottom;//下
	}Direction;

	typedef struct TopBottomLeftRightFrontBack {
		float left;//左
		float right;//右
		float top;//上
		float bottom;//下
		float front;//前
	}SurfaceDirection;

	//頂点部位
	typedef enum VerticesParts {
		LeftBottom,//左下
		LeftTop,//左上
		RightBottom,//右下
		RightTop,//右上
	}Corner;

public://基本関数
	void Initialize(ObjectBasis* objBas);
	void Update();
	void Draw();

private://固有関数
	//頂点バッファビュー作成
	void CreateVertexBufferView();
	//インデックスバッファビュー作成
	void CreateIndexBufferView();
	//定数バッファ生成
	void GenerateConstBuffer();
	//定数バッファマテリアル生成
	void GenerateConstMaterial();
	//定数バッファトランスフォーム生成
	void GenerateConstTransform();

	//ワールド行列再計算
	void ReCalcMatWorld();

public://アクセス
	///ゲッタ

	//座標を取得
	const Vector3& GetPosition() const { return worldTransform_.position; }
	//回転を取得
	const Vector3& GetRotation() const { return worldTransform_.rotation; }
	//色を取得
	const Vector4& GetColor() const { return color_; }
	//サイズを取得
	const Vector2& GetSize() const { return size_; }
	//非表示フラグを取得
	bool GetIsInvisible() const { return isInvisible_; }

	//テクスチャ番号を取得
	uint32_t GetTextureIndex() const { return textureIndex_; }

	///セッタ

	//座標をセット
	void SetPosition(const Vector3& position) { worldTransform_.position = position; }
	//回転をセット
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; }
	//色をセット
	void SetColor(const Vector4& color) { color_ = color; }
	//サイズをセット
	void SetSize(const Vector2& size) { size_ = size; }
	//非表示フラグをセット
	void SetInvisible(bool isInvisible) { isInvisible_ = isInvisible; }

	//テクスチャ番号をセット
	void SetTextureIndex(uint32_t textureIndex) { textureIndex_ = textureIndex; }

private://定数
	//頂点数
	static const int kVerticesNum = 4;
	//インデックス数
	static const int kindicesNum = 4;

private://メンバ変数
	//ワールド変換
	WorldTransform worldTransform_;
	//上下左右
	Direction dir_;

	//色
	Vector4 color_ = { 1,1,1,1 };
	//表示サイズ
	Vector2 size_ = { 100,100 };

	//非表示フラグ
	bool isInvisible_ = false;

	//オブジェクト基盤
	ObjectBasis* objBas_ = nullptr;

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	/// <summary>
	/// 頂点バッファ
	/// </summary>

	//頂点データ
	Vertex vertices_[kVerticesNum]{};
	//頂点データマップ
	Vertex* vertMap_ = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	/// <summary>
	/// インデックスバッファ
	/// </summary>

	//インデックスデータ
	uint32_t indices_[kindicesNum]{};
	//インデックスデータマップ
	uint32_t* indMap_ = nullptr;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indBuff_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	/// <summary>
	/// 定数バッファ
	/// </summary>

	//定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuffMaterial_;
	//定数バッファマテリアルマップ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	//定数バッファトランスフォーム
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファマテリアルマップ
	ConstBufferDataTransform* constMapTransform_ = nullptr;

	/// <summary>
	/// テクスチャ
	/// </summary>

	//テクスチャ番号
	uint32_t textureIndex_ = 0;
};