#pragma once

#include "json.hpp"

#include <string>

#include <vector>
#include <Vector3.h>
#include <Object3d.h>
#include <Model.h>

class Loader {
public:
	// �\����
	struct LevelData {
		struct ObjectData {
			std::string fileName_; //�I�u�W�F�N�g�t�@�C���l�[��
			Vector3 tranlation_; //���s�ړ�
			Vector3 rotation_; //��]
			Vector3 scaling_; //�X�P�[�����O
		};
		//�I�u�W�F�N�g�f�[�^�R���e�i
		std::vector<ObjectData> objects_;
	};

public:
	void LoadFileJson(const std::string fileName,Camera* camera);

	void Finalize();

	std::vector<Object3d*> GetObjects() {
		return objects_;
	}

private:
	static const std::string kDefaultBaseDirectory_;

	//���f���f�[�^�R���e�i
	std::map <std::string, Model*> models;

	//�I�u�W�F�N�g�f�[�^�R���e�i
	std::vector<Object3d*> objects_;
};