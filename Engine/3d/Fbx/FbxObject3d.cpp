#include "FbxObject3d.h"

//�f�o�C�X
static ID3D12Device* device_ = nullptr;
//�J����
static Camera* camera_ = nullptr;

void FbxObject3d::Initialize() {
	worldTransform_.Initialize();
}
