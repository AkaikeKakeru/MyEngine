#include "DrawBasis.h"
#include "Vector3.h"
#include <wrl.h>

void DrawBasis::Initialize() {
	//頂点データ
	Vector3 vertices[] = {
		{-0.5f,0.5f,0.0f},//左下
		{-0.5f,+0.5f,0.0f},//左上
		{+0.5f,-0.5f,0.0f},//右下
	};
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));
}