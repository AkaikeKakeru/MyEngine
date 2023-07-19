#pragma once
#include "SpriteBasis.h"
#include <Matrix4.h>
#include <MyMath.h>

class PostEffect {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://構造体
	  //定数バッファ用データ構造体(マテリアル)
	typedef struct ConstBufferDataMaterial {
		Vector4 color_ = { 0,0,0,0 };//色(RGBA)
	}ConstBufferDataMaterial;

	//定数バッファ用データ構造体(3D変換行列)
	typedef struct ConstBufferDataTransform {
		Matrix4 mat_ = Matrix4Identity();//3D変換行列
	}ConstBufferDataTransform;

	//ワールド変換構造体
	typedef struct WorldTransform {
		Vector3 scale_ = { 0,0,0 };		//スケール
		float rotation_ = 0.0f;	//回転
		Vector2 position_ = { 0,0 };//平行移動
		Matrix4 matWorld_ = Matrix4Identity();	//ワールド変換行列
	}WorldTransform;

	//頂点データ構造体
	typedef struct VertexPosUv {
		Vector3 pos = { 0,0,0 };//xyz座標
		Vector2 uv = { 0,0 };//uv座標
	}Vertex;

	typedef struct TopBottomLeftRight {
		float left;//左
		float right;//右
		float top;//上
		float bottom;//下
	}Direction;

	//頂点部位
	typedef enum VerticesParts {
		LeftBottom,//左下
		LeftTop,//左上
		RightBottom,//右下
		RightTop,//右上
	}VerticesParts;

	typedef struct Texture {
		//ワールド変換
		WorldTransform worldTransform_;
		//上下左右
		Direction dir_ = { 0,0,0,0 };

		//色
		Vector4 color_ = { 1,1,1,1 };
		//表示サイズ
		Vector2 size_ = { 100,100 };
		//アンカーポイント
		Vector2 anchorPoint_ = { 0,0 };
		//左右反転フラグ
		bool isFlipX_ = false;
		//上下反転フラグ
		bool isFlipY_ = false;
		//非表示フラグ
		bool isInvisible_ = false;
		//テクスチャ左上座標
		Vector2 textureLeftTop_ = { 0.0f,0.0f };
		//テクスチャ切り出しサイズ
		Vector2 textureSize_ = { 100.0f,100.0f };

		//平行投影変換行列
		Matrix4 matOrtGrapricProjection_ = Matrix4Identity();
	}Texture;

public://メンバ関数
	//初期化
	void Initialize();
	//描画コマンド実行
	void Draw();

	//頂点バッファビュー作成
	void CreateVertexBufferView();
	//定数バッファ生成
	void GenerateConstBuffer();
	//定数バッファマテリアル生成
	void GenerateConstMaterial();
	//定数バッファトランスフォーム生成
	void GenerateConstTransform();

	//ワールド行列再計算
	void ReCalcMatWorld();

	//テクスチャ描画事前処理
	void PreDraw();
	//テクスチャセットコマンド
	void TextureCommand();

	//シーン描画前処理
	void PreDrawScene();
	//シーン描画後処理
	void PostDrawScene();

	//テクスチャバッファ生成
	void GenerateTextureBuffer();
	//テクスチャバッファへ転送
	void TransferTextureBuffer();
	//SRV作成
	void CreateSRV();
	//RTV作成
	void CreateRTV();
	//深度バッファ生成
	void GenerateDepthBuffer();
	//DSV作成
	void CreateDSV();

private:
	//頂点数
	static const int kVerticesNum = 4;

private://静的メンバ変数
	//画面クリアカラー
	static const float clearColor_[4];

private://メンバ変数
	//テクスチャ
	Texture texture_;

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;

	//頂点データ
	Vertex vertices_[kVerticesNum]{};
	//頂点データマップ
	Vertex* vertMap_ = nullptr;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

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
	//シェーダーリソースビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff_;
	//レンダ―ターゲットビュー用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
	//深度ステンシル用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV_;

	//リソースバリアデスク
	D3D12_RESOURCE_BARRIER barrierDesc_{};

public://コンストラクタ
	PostEffect();
};
