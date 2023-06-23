#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>

#include <string>
#include "Vector3.h"
#include "Matrix4.h"
#include <vector>
#include <DirectXTex.h>

#include <fbxsdk.h>

//�m�[�h�\����
struct Node {
	//���O
	std::string name_;

	//���[�J���X�P�[��
	Vector3 scaling_ = { 1,1,1 };

	//���[�J����]
	Vector3 rotation_ = { 0,0,0 };

	//���[�J���ړ�
	Vector3 translation_ = { 0,0,0 };

	//���[�J���ό`�s��
	Matrix4 transform_;

	//�O���[�o���ό`�s��
	Matrix4 globalTransform_;

	//�e�m�[�h
	Node* parent_ = nullptr;
};

//Fbx���f��
class FbxModel {
private://�ȗ�
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T> using vector = std::vector <T>;

public:
	//�t�����h�N���X
	friend class FbxLoader;

public://�萔
	//�{�[���C���f�b�N�X�̍ő�l
	static const int MAX_BONE_INDICES_ = 4;

public://�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPosNormalUvSkin {
		Vector3 pos_;
		Vector3 normal_;
		Vector3 uv_;
		UINT boneIndex_[MAX_BONE_INDICES_]; //�{�[�� �ԍ�
		float boneWeight_[MAX_BONE_INDICES_]; //�{�[�� �d��
	};

	//�{�[���\����
	struct Bone {
		//���O
		std::string name_;

		//�����p���̋t�s��
		Matrix4 invInitialPose_;

		//�N���X�^�[(FBX���̃{�[�����)
		FbxCluster* fbxCluster_;

		//�R���X�g���N�^
		Bone(const std::string& name) {
			this->name_ = name;
		}
	};

public://�����o�֐�
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	   
	//�o�b�t�@����
	void CreateBuffers(ID3D12Device* device);

public://�A�N�Z�b�T
	//���f���̕ό`�s��擾
	const Matrix4& GetModelTransform() {
		return meshNode->globalTransform_;
	}

	//�{�[���z��̎擾
	std::vector<Bone>& GetBones() {
		return bones_;
	}
	 
	//FBX�V�[���̎擾
	FbxScene* GetFbxScene() {
		return fbxScene_;
	}

private://�����o�ϐ�
	//FBX�V�[��
	FbxScene* fbxScene_ = nullptr;

	///�{�[���֌W

	//�{�[���z��
	std::vector<Bone> bones_;

	///���f���֌W

	//���f����
	string name_;
	//�m�[�h�z��
	vector<Node> nodes_;
	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//���_�f�[�^�z��
	vector<VertexPosNormalUvSkin> vertices_;
	//���_�C���f�b�N�X�z��
	vector<unsigned short> indices_;

	///�}�e���A���֌W

	//�A���r�G���g�W��
	Vector3 ambient_ = { 1,1,1 };
	//�f�B�t���[�Y�W��
	Vector3 diffuse_ = { 1,1,1 };
	//�e�N�X�`�����^�f�[�^
	TexMetadata metadata = {};
	//�X�N���b�`�C���[�W
	ScratchImage scratchImg_ = {};

	///�o�b�t�@�֌W

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff_;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	//SRV�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

public: //�f�X�g���N�^
	~FbxModel();
};