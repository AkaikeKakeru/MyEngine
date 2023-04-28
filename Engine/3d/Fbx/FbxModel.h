#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>

#include <string>
#include "Vector3.h"
#include "Matrix4.h"
#include <vector>
#include <DirectXTex.h>

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

public://�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPosNormalUv {
		Vector3 pos_;
		Vector3 normal_;
		Vector3 uv_;
	};

public://�����o�֐�
	//�o�b�t�@����
	void CreateBuffers(ID3D12Device* device);

private://�����o�ϐ�
	///���f���֌W

	//���f����
	string name_;
	//�m�[�h�z��
	vector<Node> nodes_;
	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//���_�f�[�^�z��
	vector<VertexPosNormalUv> vertices_;
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
};