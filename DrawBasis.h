#pragma once
#include "DirectXBasis.h"

class DrawBasis {
public://基本関数
	void Initialize();

private://固有関数
	void CreateVertexBufferView();

private://メンバ変数
	DirectXBasis* dxBas_ = nullptr;
};