﻿#include "FbxLoader.h"
#include <cassert>

//静的メンバ変数の実体
const std::string FbxLoader::BaseDirectory_ = "Resource/";

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

void FbxLoader::LoadModelFromFile(const string& modelName){
    //Resource + modelName + / 
    const string directoryPath = BaseDirectory_ + modelName + "/";
    //modelName.fbx
    const string fileName = modelName + ".fbx";
    //(Resource + modelName + /) + (modelName.fbx)
    const string fullpath = directoryPath + fileName;

    //ファイル名で指定して、FBXファイルを読み込む
    if (!fbxImporter_->Initialize(fullpath.c_str(),
        -1, fbxManager_->GetIOSettings())) {
        assert(0);
    }

    //シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");

    //ファイルからロードしたFBXの情報をシーン化したインポート
    fbxImporter_->Import(fbxScene);
}
