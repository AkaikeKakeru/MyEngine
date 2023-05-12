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
protected: //�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //�T�u�N���X

public: //�ÓI�����o�֐�
	//�f�o�C�X�Z�b�g
	static void SetDevice(ID3D12Device* device) {
		FbxObject3d::device_;
	}
	//�J�����Z�b�g
	static void SetCamera(Camera* camera) {
		FbxObject3d::camera_;
	}

public: //�����o�֐�
	//������
	void Initialize();

private: //�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device_;
	//�J����
	static Camera* camera_;

protected: //�����o�ϐ�
	WorldTransform worldTransform_;
};