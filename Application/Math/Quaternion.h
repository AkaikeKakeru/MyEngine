#pragma once
#include "Vector3.h"
#include "Matrix4.h"

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

Quaternion& operator/(Quaternion q, float s);
Quaternion& operator/=(Quaternion q, float s);

//Quaternionの積
Quaternion QuaternionMultiplication(const Quaternion& q,const Quaternion& r);
//Quaternionの単位化
Quaternion QuaternionIdentity();
//Quaternionの共役
Quaternion QuaternionConjugation(const Quaternion& quaternion);
//Quaternionのノルム(長さ)
float QuaternionNorm(const Quaternion& quaternion);
//Quaternionの正規化
Quaternion QuaternionNormalize(const Quaternion& quaternion);
//逆Quaternion
Quaternion QuaternionInverse(const Quaternion& quaternion);

//任意軸回転を表すQuaternionの生成
Quaternion MakeAxisAngle(const Vector3& axis, float angle);

//ベクトルをQuaternionで回転させた結果のベクトル
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

//Quaternionから回転行列を求める
Matrix4 MakeRotateMatrix(const Quaternion& quaternion);