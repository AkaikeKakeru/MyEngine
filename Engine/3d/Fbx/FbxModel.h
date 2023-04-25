#pragma once
#include <string>
#include "Vector3.h"
#include "Matrix4.h"

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

private:
	//���f����
	std::string name_;
	//�m�[�h�z��
	std::vector<Node> nodes_;
};