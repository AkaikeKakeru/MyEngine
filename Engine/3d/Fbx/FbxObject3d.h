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

#include "LightGroup.h"

class FbxObject3d {
protected: //�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //�萔
	// �{�[���̍ő吔
	static const int MAX_BONES = 32;

public: //�T�u�N���X
	// �萔�o�b�t�@�p�f�[�^�\����(�X�L�j���O)
	struct ConstBufferDataSkin {
		Matrix4 bones[MAX_BONES];
	};

public: //�ÓI�����o�֐�
	//�O���t�B�b�N�X�p�C�v���C������
	static void CreateGraphicsPipeline();

public: //�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//���[���h�s��]��
	void TransferMatrixWorld();

public: //�A�N�Z�b�T
	//�f�o�C�X�Z�b�g
	static void SetDevice(ID3D12Device* device) {
		FbxObject3d::device_ = device;
	}
	//�J�����Z�b�g
	static void SetCamera(Camera* camera) {
		FbxObject3d::camera_ = camera;
	}

	//���C�g�Z�b�g
	static void SetLight(LightGroup* lightGroup) {
		FbxObject3d::lightGroup_ = lightGroup;
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

	//���C�g
	static LightGroup* lightGroup_;

protected: //�����o�ϐ�
	//���[���h�ϊ�
	WorldTransform worldTransform_;

	//���f��
	FbxModel* model_ = nullptr;

	//�萔�o�b�t�@(�X�L��)
	ComPtr<ID3D12Resource> constBuffSkin_;
};