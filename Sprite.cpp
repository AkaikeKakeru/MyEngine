#include "Sprite.h"
#include <cassert>

void Sprite::Initialize(DrawBasis* drawBas){
	assert(drawBas);
	drawBas_ = drawBas;
	cmdList_ = drawBas_->GetCommandList();
}

void Sprite::Draw(){
	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}