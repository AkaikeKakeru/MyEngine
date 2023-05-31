#include "Loader.h"
#include <json.hpp>
#include <cassert>
#include <fstream>
#include <map>
#include <MyMath.h>
#include <SafeDelete.h>


const std::string Loader::kDefaultBaseDirectory_ = "Resource/levels/";

void Loader::LoadFileJson(const std::string fileName,Camera* camera) { 
	////�A�����ăt���p�X�𓾂�
	//const std::string fullpath =
	//	std::string("Resource/levels/") + "rimuta_TL1" + ".json";

	//�A�����ăt���p�X�𓾂�
	const std::string fullpath =
		kDefaultBaseDirectory_ + fileName + ".json";

	//�t�@�C���X�g���[��
	std::ifstream file;

	//�t�@�C�����J��
	file.open(fullpath);
	//�t�@�C���I�[�v���̎��s���m�F
	assert(!file.fail());

	//�𓀂����f�[�^
	nlohmann::json deserialized;

	//��
	file >> deserialized;

	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"�𕶎���Ƃ��Ď擾
	std::string name =
		deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	//���x���f�[�^�i�[�p�C���X�^���X�𐶐�
	LevelData* levelData = new LevelData();

	//����
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		//��ʂ��擾
		std::string type = object["type"].get<std::string>();

		//���b�V��
		if (type.compare("MESH") == 0) {
			//�v�f�ǉ�
			levelData->objects_.emplace_back(LevelData::ObjectData{});
			//���ǉ������v�f�̎Q�Ƃ𓾂�
			LevelData::ObjectData& objectData = levelData->objects_.back();

			if (object.contains("file_name")) {
				//�t�@�C����
				objectData.fileName_ = object["file_name"];
			}

			//�g�����X�t�H�[���̃p�����[�^�Ǎ�
			nlohmann::json& transform = object["transform"];

			//���s�ړ�
			objectData.tranlation_.x = (float)transform["translation"][1];
			objectData.tranlation_.y = (float)transform["translation"][2];
			objectData.tranlation_.z = -(float)transform["translation"][0];

			//��]�p
			objectData.rotation_.x = -(float)ConvertToRadian( transform["rotation"][1] );
			objectData.rotation_.y = -(float)ConvertToRadian( transform["rotation"][2] );
			objectData.rotation_.z = (float)ConvertToRadian(transform["rotation"][0]);

			//�X�P�[�����O
			objectData.scaling_.x = (float)transform["scaling"][1];
			objectData.scaling_.y = (float)transform["scaling"][2];
			objectData.scaling_.z = (float)transform["scaling"][0];
		}

		//TODO: �I�u�W�F�N�g�������ċA�֐��ɂ܂Ƃ߁A�ċA�֐��Ŏ}�𑖍�����
		if (object.contains("children")) {

		}
	}

	//���x���f�[�^�ɏo�����郂�f���̓Ǎ�
	for (auto& objectData : levelData->objects_) {
		//�t�@�C��������o�^�ς݃��f��������
		decltype(models)::iterator it = models.find(objectData.fileName_);

		//���Ǎ��̏ꍇ�ǂݍ���
		Model* model = Model::LoadFromOBJ(objectData.fileName_);
		models[objectData.fileName_] = model;
	}

	//���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for(auto& objectData : levelData->objects_){
		//�t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName_);
		if (it != models.end()) { model = it->second; }
		//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		Object3d* newObject = Object3d::Create();
		newObject->SetModel(model);
		//���W
		Vector3 pos;
		newObject->SetPosition(objectData.tranlation_);
		//��]�p
		newObject->SetRotation(objectData.rotation_);
		//�X�P�[��
		newObject->SetScale(objectData.scaling_);

		//�J�����Z�b�g
		newObject->SetCamera(camera);

		//�z��ɓo�^
		objects_.push_back(newObject);
	}
}

void Loader::Finalize() {
	for (Object3d* object : objects_) {
		SafeDelete(object);
	}
}
