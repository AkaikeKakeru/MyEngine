#pragma once

#include "FbxModel.h"
#include "Camera.h"
#include "WorldTransform.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <string>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "MyMath.h"

class FbxObject3d {
protected: //省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //サブクラス

public: //静的メンバ関数
	//デバイスセット
	static void SetDevice(ID3D12Device* device) {
		FbxObject3d::device_;
	}
	//カメラセット
	static void SetCamera(Camera* camera) {
		FbxObject3d::camera_;
	}

public: //メンバ関数
	//初期化
	void Initialize();

private: //静的メンバ変数
	//デバイス
	static ID3D12Device* device_;
	//カメラ
	static Camera* camera_;

protected: //メンバ変数
	WorldTransform worldTransform_;
};