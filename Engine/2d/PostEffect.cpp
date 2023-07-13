#include "PostEffect.h"

void PostEffect::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	//パイプラインステート設定
	cmdList_->SetPipelineState(drawBas_->GetPipelineState().Get());
	//ルートシグネチャ設定
	cmdList_->SetGraphicsRootSignature(drawBas_->GetRootSignature().Get());
	//プリミティブ形状設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//描画用テクスチャコマンド
	drawBas_->SetTextureCommand(textureIndex_);

	//頂点バッファビューの設定コマンド
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//インスタンス描画
	cmdList_->DrawInstanced(kVerticesNum, 1, 0, 0);
}

PostEffect::PostEffect()
	:Sprite(
		100,
		{ 0, 0 },
		{ 500.0f, 500.0f },
		{ 1,1,1,1 },
		{ 0.0f, 0.0f },
		false,
		false) {
}
