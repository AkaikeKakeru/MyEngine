#pragma once
#include "Sprite.h"
class PostEffect :
    public Sprite {
public://�����o�֐�
    //�`��R�}���h���s
    void Draw(ID3D12GraphicsCommandList* cmdList);
public://�R���X�g���N�^
    PostEffect();
};

