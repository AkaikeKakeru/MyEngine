#pragma once
#include <string>

//�m�[�h�\����
struct Node {

};

//Fbx���f��
class FbxModel {
public:
	//�t�����h�N���X
	friend class FbxLoader;

private:
	//���f����
	std::string name_;
};