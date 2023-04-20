#include "FbxLoader.h"
#include <cassert>

FbxLoader* FbxLoader::GetInstance() {
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::Initialize(ID3D12Device* device) {
    // 再初期化チェック
    assert(fbxManager_ == nullptr);
    // 引数からメンバ変数に代入
    device_ = device;
    // FBXマネージャーの生成
    fbxManager_ = FbxManager::Create();
    // FBXマネージャーの入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager_, IOSROOT);
    fbxManager_->SetIOSettings(ios);
    // FBXインポータの生成
    fbxImporter_ = FbxImporter::Create(fbxManager_, "");
}

void FbxLoader::Finalize() {
    // 各種FBXインスタンスの破棄
    fbxImporter_->Destroy();
    fbxManager_->Destroy();
}