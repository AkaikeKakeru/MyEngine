#include "FbxObject3d.h"

//デバイス
static ID3D12Device* device_ = nullptr;
//カメラ
static Camera* camera_ = nullptr;

void FbxObject3d::Initialize() {
	worldTransform_.Initialize();
}
