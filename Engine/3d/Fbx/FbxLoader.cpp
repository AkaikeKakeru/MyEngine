#include "FbxLoader.h"
#include <cassert>
#include "MyMath.h"

using string = std::string;

//静的メンバ変数の実体
const std::string FbxLoader::BaseDirectory_ = "Resource/";

FbxLoader* FbxLoader::GetInstance() {
	static FbxLoader instance;
	return &instance;
}

void FbxLoader::Initialize(ID3D12Device* device) {
	// 再初期化チェック
	assert(fbxManager_ == nullptr);
	// 引数からメンバ変数に代入
	device_ = device;
	// FBXマネージャーの生成
	fbxManager_ = FbxManager::Create();
	// FBXマネージャーの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager_, IOSROOT);
	fbxManager_->SetIOSettings(ios);
	// FBXインポータの生成
	fbxImporter_ = FbxImporter::Create(fbxManager_, "");
}

void FbxLoader::Finalize() {
	// 各種FBXインスタンスの破棄
	fbxImporter_->Destroy();
	fbxManager_->Destroy();
}

void FbxLoader::LoadModelFromFile(const string& modelName) {
	//Resource + modelName + / 
	const string directoryPath = BaseDirectory_ + modelName + "/";
	//modelName.fbx
	const string fileName = modelName + ".fbx";
	//(Resource + modelName + /) + (modelName.fbx)
	const string fullpath = directoryPath + fileName;

	//ファイル名で指定して、FBXファイルを読み込む
	if (!fbxImporter_->Initialize(fullpath.c_str(),
		-1, fbxManager_->GetIOSettings())) {
		assert(0);
	}

	//シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");

	//ファイルからロードしたFBXの情報をシーン化したインポート
	fbxImporter_->Import(fbxScene);

	// モデル生成
	FbxModel* model = new FbxModel();
	model->name_ = modelName;

	//FBXノードの数を取得
	int nodeCount = fbxScene->GetNodeCount();

	//あらかじめ必要数分のメモリを確保することで、アドレスがズレるのを予防
	model->nodes_.reserve(nodeCount);

	// ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(model, fbxScene->GetRootNode());

	// FBXシーン解放
	fbxScene->Destroy();
}

void FbxLoader::ParseNodeRecursive(
	FbxModel* model, FbxNode* fbxNode, Node* parent) {
	// モデルにノードを追加 (Todo)
	model->nodes_.emplace_back();
	Node& node = model->nodes_.back();

	// ノード名を取得
	node.name_ = fbxNode->GetName();

	/// FBXノードの情報を解析してノードに記録

	//FBXノードのローカル移動情報
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	//形式変換して代入
	node.rotation_ = { (float)rotation[0],(float)rotation[1],(float)rotation[2] };
	node.scaling_ = { (float)scaling[0],(float)scaling[1],(float)scaling[2] };
	node.translation_ = { (float)translation[0],(float)translation[1],(float)translation[2] };

	//回転角をDegreeからラジアンへ変換
	node.rotation_ = {
		ConvertToRadian(node.rotation_.x),
		ConvertToRadian(node.rotation_.y),
		ConvertToRadian(node.rotation_.z)
	};

	//スケール、回転、平行移動行列の計算
	Matrix4 matS = Matrix4Identity(),
		matR = Matrix4Identity(),
		matT = Matrix4Identity();
	matS = Matrix4Scale(node.scaling_);
	matR = Matrix4Rotation(node.rotation_);
	matT = Matrix4Translation(node.translation_);

	//ローカル変換行列の計算
	node.transform_ = Matrix4Identity();
	node.transform_ = matS *= matR *= matT;

	//グローバル変換行列の計算
	node.globalTransform_ = node.transform_;
	if (parent) {
		node.parent_ = parent;
		//親の変形を乗算
		node.globalTransform_ *= parent->globalTransform_;
	}

	// FBXノードのメッシュ情報を解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute) {
		if (fbxNodeAttribute->GetAttributeType() ==
			FbxNodeAttribute::eMesh) {
			model->meshNode = &node;
			ParseMesh(model, fbxNode);
		}
	}

	//子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++) {
		ParseNodeRecursive(model, fbxNode->GetChild(i));
	}
}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode) {
	//ノードのメッシュを取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//頂点座標読み取り
	ParseMeshVertices(model, fbxMesh);
	//面を構成するデータの読み取り
	ParseMeshFaces(model, fbxMesh);
	//マテリアルの読み取り
	ParseMaterial(model, fbxNode);
}

void FbxLoader::ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh) {
	auto& vertices = model->vertices_;

	//頂点座標データの数
	const int controlPointsCount =
		fbxMesh->GetControlPointsCount();
	
	//必要数だけ頂点データ配列を確保
	FbxModel::VertexPosNormalUv vert{};
	model->vertices_.resize(controlPointsCount, vert);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4* pCoord = fbxMesh->GetControlPoints();

	//FBXメッシュの全頂点座標をモデル内の配列にコピーする
	for (int i = 0; i < controlPointsCount; i++) {
		FbxModel::VertexPosNormalUv& vertex = vertices[i];

		//座標のコピー
		vertex.pos_.x = (float)pCoord[i][0];
		vertex.pos_.y = (float)pCoord[i][1];
		vertex.pos_.z = (float)pCoord[i][2];
	}
}

void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh) {
	auto& vertices = model->vertices_;
	auto& indices = model->indices_;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);
	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();
	//UVデータの数
	const int textureUVCount = fbxMesh->GetTextureUVCount();
	//UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//面ごとの位置読み取り
	for(int i = 0; i < polygonCount; i++) {
		//面を構成する頂点の数を取得(3なら三角形ポリゴン)
		const int porygonSize = fbxMesh->GetPolygonSize(i);
		assert(porygonSize <= 4);

		//頂点ずつ処理
		for (int j = 0; j < porygonSize; j++) {
			//FBX頂点配列のインデックス
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			//頂点法線読込
			FbxModel::VertexPosNormalUv& vertex = vertices[index];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
				vertex.normal_.x = (float)normal[0];
				vertex.normal_.y = (float)normal[1];
				vertex.normal_.z = (float)normal[2];
			}

			//テクスチャの読込
			if (textureUVCount > 0) {
				FbxVector2 uvs;
				bool lUnmappedUV = false;

				//0番決め打ちで読込
				if (fbxMesh->GetPolygonVertexUV(i, j,
					uvNames[0], uvs, lUnmappedUV)) {
					vertex.uv_.x = (float)uvs[0];
					vertex.uv_.y = (float)uvs[1];
				}
			}
		}
	}
}
