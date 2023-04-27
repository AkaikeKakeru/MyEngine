#pragma once
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

private:
	//���f����
	std::string name_;
	//�m�[�h�z��
	std::vector<Node> nodes_;
	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices_;
	//���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;

	//�A���r�G���g�W��
	Vector3 ambient_ = { 1,1,1 };
	//�f�B�t���[�Y�W��
	Vector3 diffuse_ = { 1,1,1 };
	//�e�N�X�`�����^�f�[�^
	DirectX::TexMetadata metadata = {};
	//�X�N���b�`�C���[�W
	DirectX::ScratchImage scratchImg_ = {};
};