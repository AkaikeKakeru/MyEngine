#include "PostEffect.h"

void PostEffect::Draw() {
	//��\��
	if (isInvisible_) {
		return;
	}

	//�p�C�v���C���X�e�[�g�ݒ�
	cmdList_->SetPipelineState(drawBas_->GetPipelineState().Get());
	//���[�g�V�O�l�`���ݒ�
	cmdList_->SetGraphicsRootSignature(drawBas_->GetRootSignature().Get());
	//�v���~�e�B�u�`��ݒ�
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�`��p�e�N�X�`���R�}���h
	drawBas_->SetTextureCommand(textureIndex_);

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	//�C���X�^���X�`��
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
