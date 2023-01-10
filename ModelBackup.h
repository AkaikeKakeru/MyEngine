#pragma once
#include "ObjectBasis.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

class ModelBackup {
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
		Vector3 position;	//平行移動
		Matrix4 matWorld = Matrix4Identity();	//ワールド変換行列
	}WorldTransform;

	//プロジェクション構造体
	typedef struct ViewProjection {
		Vector3 eye{ 0,0,-100 };	//視点座標
		Vector3 target{ 0,0,0 };	//注視点座標
		Vector3 up{ 0,1,0 };	//上方向ベクトル

		float angle;	//視野角
		float aspect;	//アスペクト比
		float nearClip;	//ニア
		float farClip;	//ファー

		Matrix4 matView{};	//	ビュー行列
		Matrix4 matPerspective{};	//透視投影行列
	}ViewProjection;

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
		float back;//後
	}SurfaceDirection;

	//頂点部位
	typedef enum VerticesParts {
		//前
		LeftBottomFront,//左下
		LeftTopFront,//左上
		RightBottomFront,//右下
		RightTopFront,//右上
					  //後ろ
					  LeftBottomBack,//左下
					  LeftTopBack,//左上
					  RightBottomBack,//右下
					  RightTopBack,//右上
								   //左
								   LeftBottomLeft,//左下
								   LeftTopLeft,//左上
								   RightBottomLeft,//右下
								   RightTopLeft,//右上
												//右
												LeftBottomRight,//左下
												LeftTopRight,//左上
												RightBottomRight,//右下
												RightTopRight,//右上
															  //下
															  LeftBottomBottom,//左下
															  LeftTopBottom,//左上
															  RightBottomBottom,//右下
															  RightTopBottom,//右上
																			 //上
																			 LeftBottomTop,//左下
																			 LeftTopTop,//左上
																			 RightBottomTop,//右下
																			 RightTopTop,//右上
	}Corner;

public://基本関数
	void Initialize(ObjectBasis* objBas);
	void Update();
	void Draw();

	//カメラ視点移動
	static void CameraMoveEye(Vector3 move);
	//ビュー行列再計算
	static void ReCalcMatView();

private://固有関数
	void InitWorldTransform();
	void InitView();
	void InitProjection();

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

	static const Vector3& GetEye() { return viewProjection_.eye; };
	static const Vector3& GetTarget() { return viewProjection_.target; };

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

	static void SetEye(const Vector3& eye) {
		viewProjection_.eye = eye;
		ReCalcMatView();
	};

private://定数
		//三角形頂点数
	static const int kTriangleNum = 3;
	//面数
	static const int kSurfaceNum = 6;
	//頂点数
	static const int kVerticesNum = 4 * kSurfaceNum;
	//インデックス数
	static const int kIndicesNum = 2 * kTriangleNum*kSurfaceNum;

private://メンバ変数
		//ワールド変換
	WorldTransform worldTransform_;
	//上下左右前後
	SurfaceDirection dir_;

	//プロジェクション
	static ViewProjection viewProjection_;

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
	unsigned short indices_[kIndicesNum]{};
	//インデックスデータマップ
	uint16_t* indMap_ = nullptr;
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