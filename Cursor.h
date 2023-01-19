#pragma once
#include "Vector2.h"
#include "Matrix4.h"

#include "Object3d.h"

class Corsor {
public:
	Vector3 Get3DRethiclePosition(const Object3d* object,
		const float distance, const bool isOnScreen);

private:

void GetMousePosition();
void CreateMatrixInverseViewPort(const Object3d* object);
void CheckRayDirection();

private:
	Matrix4 matInverseVPV_ = Matrix4Identity();
	Vector2 mousePosition_ = {};

	Vector3 posNear_ = {};
	Vector3 posFar_ = {};
	Vector3 rayDirection_ = {};
};