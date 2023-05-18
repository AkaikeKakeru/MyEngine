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
	//�O���t�B�b�N�X�p�C�v���C������
	static void CreateGraphicsPipeline();

public: //�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();

public: //�A�N�Z�b�T
	//�f�o�C�X�Z�b�g
	static void SetDevice(ID3D12Device* device) {
		FbxObject3d::device_;
	}
	//�J�����Z�b�g
	static void SetCamera(Camera* camera) {
		FbxObject3d::camera_;
	}

	//���f���Z�b�g
	void SetModel(FbxModel* model) {
		model_ = model;
	}

private: //�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device_;

	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature_;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate_;

	//�J����
	static Camera* camera_;

protected: //�����o�ϐ�
	//���[���h�ϊ�
	WorldTransform worldTransform_;

	//���f��
	FbxModel* model_ = nullptr;
};