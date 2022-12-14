#pragma once
#include "DrawBasis.h"
#include "Vector3.h"

class Sprite {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体
	typedef struct VertexPos {
		Vector3 pos;
	} VertexPos;

public://基本関数
	void Initialize(DrawBasis* drawBas);
	void Draw();

private://固有関数

private://定数
private://メンバ変数
		//Draw基盤
	DrawBasis* drawBas_ = nullptr;

	//デバイス
	ComPtr<ID3D12Device> device_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;
};