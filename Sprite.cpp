#include "Sprite.h"
#include <cassert>

void Sprite::Initialize(DrawBasis* drawBas) {
	assert(drawBas);
	drawBas_ = drawBas;
	cmdList_ = drawBas_->GetCommandList();
	vbView_ = drawBas_->GetVertexBufferView();
}

void Sprite::Draw() {
	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}