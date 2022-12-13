#pragma once
#include "DirectXBasis.h"

class DrawBasis {
public://基本関数
	void Initialize();

private://固有関数
	//頂点バッファビュー作成
	void CreateVertexBufferView();
	//シェーダーファイルのコンパイル
	void CompileShaderFile();
	//頂点レイアウト
	void AssembleVertexLayout();

private://メンバ変数
	DirectXBasis* dxBas_ = nullptr;
};