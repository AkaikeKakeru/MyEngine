#pragma once
#include "Sprite.h"
class PostEffect :
    public Sprite {
public://メンバ関数
    //描画コマンド実行
    void Draw();
public://コンストラクタ
    PostEffect();
};

