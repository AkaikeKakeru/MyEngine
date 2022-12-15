#pragma once
#include "DrawBasis.h"
#include "Matrix4.h"
#include "Vector3.h"

class Sprite {
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
		float rotation = 0.0f;	//回転
		Vector2 position;//平行移動
		Matrix4 matWorld = Matrix4Identity();	//ワールド変換行列
	}WorldTransform;

public://基本関数
	void Initialize(DrawBasis* drawBas);
	void Draw();

private://固有関数
	//定数バッファ生成
	void GenerateConstBuffer();
	//定数バッファマテリアル生成
	void GenerateConstMaterial();
	//定数バッファトランスフォーム生成
	void GenerateConstTransform();

	//テクスチャバッファ生成
	void GenerateTextureBuffer();
	//デスクリプタヒープ生成
	void GenerateDescriptorHeap();
	//シェーダーリソースビュー作成
	void CreateShaderResourceView();

	//ワールド行列再計算
	void ReCalcMatWorld();

public://アクセス
	//座標を取得
	const Vector2& GetPosition() const {
		return worldTransform_.position;
	}

	//回転を取得
	float GetRotation() const {
		return worldTransform_.rotation;
	}

	//座標をセット
	void SetPosition(const Vector2& position) {
		worldTransform_.position = position;
	}

	//回転をセット
	void SetRotation(float rotation) {
		worldTransform_.rotation = rotation;
	}

private://定数
	//頂点数
	static const int kVerticesNum = 4;

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

private://メンバ変数
	//ワールド変換
	WorldTransform worldTransform_;

	//平行投影変換行列
	Matrix4 matOrtGrapricProjection_;

	//Draw基盤
	DrawBasis* drawBas_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	//定数バッファマテリアル
	ComPtr<ID3D12Resource> constBuffMaterial_;
	//定数バッファマテリアルマップ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	//定数バッファトランスフォーム
	ComPtr<ID3D12Resource> constBuffTransform_;
	//定数バッファマテリアルマップ
	ConstBufferDataTransform* constMapTransform_ = nullptr;

	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;

	//テクスチャリソースデスク
	D3D12_RESOURCE_DESC texResDesc_{};

	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;

	//SRVヒープのアドレス
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
};