#include "Cursor.h"
#include "Input.h"

Vector3 Corsor::Get3DRethiclePosition(const Object3d* object,
	const float distance, const bool isOnScreen){
	GetMousePosition();
	CreateMatrixInverseViewPort(object);
	CheckRayDirection();

	Vector3 position = object->GetPosition();

	position = rayDirection_ * distance;

	if (isOnScreen) {
		position.z -= distance;
	}

	return position;
}

void Corsor::GetMousePosition() {
	//マウスの座標を取得
	mousePosition_ = Input::GetInstance()->GetMousePosition();
}

void Corsor::CreateMatrixInverseViewPort(const Object3d* object){
	Matrix4 matViewPort = Matrix4Identity();
	matViewPort.m[0][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[1][1] = static_cast<float>(-(WinApp::Win_Height)) / 2;
	matViewPort.m[3][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[3][1] = static_cast<float>(WinApp::Win_Height) / 2;

	Matrix4 matVPV = object->GetViewProjection().matView_
		* object->GetViewProjection().matProjection_
		* matViewPort;

	//上を逆行列化
	matInverseVPV_ = Matrix4Inverse(matVPV);
}

void Corsor::CheckRayDirection(){
	//ニア
	posNear_ = Vector3(
		mousePosition_.x,
		mousePosition_.y,
		0);
	posNear_ = Vector3Transform(posNear_, matInverseVPV_);
	//ファー
	posFar_ = Vector3(
		mousePosition_.x,
		mousePosition_.y,
		1);
	posFar_ = Vector3Transform(posFar_, matInverseVPV_);

	//レイ
	rayDirection_ = posFar_ - posNear_;

	//ニア→レイ
	rayDirection_ = rayDirection_ - posNear_;
	rayDirection_ = Vector3Normalize(rayDirection_);
}
