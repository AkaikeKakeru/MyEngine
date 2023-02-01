#include "Quaternion.h"
#include <cmath>

Quaternion& operator/(Quaternion q, float s) {
	q.x /= s;
	q.y /= s;
	q.z /= s;
	q.w /= s;

	return q;
};

Quaternion& operator/=(Quaternion q, float s) {
	q.x /= s;
	q.y /= s;
	q.z /= s;
	q.w /= s;

	return q;
};

Quaternion QuaternionMultiplication(const Quaternion& q, const Quaternion& r) {
	Quaternion qr = {
		q.w * r.x - q.z * r.y + q.y * r.z + q.x * r.w,
		q.z * r.x + q.w * r.y - q.x * r.z + q.y * r.w,
		-q.y * r.x + q.x * r.y + q.w * r.z + q.z * r.w,
		-q.x * r.x - q.y * r.y - q.z * r.z + q.w * r.w
	};

	return qr;
}

Quaternion QuaternionIdentity() {
	Quaternion identity = {
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	return identity;
}

Quaternion QuaternionConjugation(const Quaternion& quaternion) {
	Quaternion conjugation = {
		-quaternion.x,
		-quaternion.y,
		-quaternion.z,
		quaternion.w
	};

	return conjugation;
}

float QuaternionNorm(const Quaternion& quaternion) {
	float norm =
		sqrt((quaternion.w * quaternion.w)
			+ (quaternion.x * quaternion.x)
			+ (quaternion.y * quaternion.y)
			+ (quaternion.z * quaternion.z)
		);

	return norm;
}

Quaternion QuaternionNormalize(const Quaternion& quaternion) {
	float len = QuaternionNorm(quaternion);

	if (len != 0) {
		return quaternion /= len;
	}

	return quaternion;
}

Quaternion QuaternionInverse(const Quaternion& quaternion) {
	Quaternion conj = QuaternionConjugation(quaternion);
	float norm = QuaternionNorm(quaternion);

	Quaternion inverse = conj / (norm * norm);

	return inverse;
}

Quaternion MakeAxisAngle(const Vector3& axis, float angle) {
	float c = cos(angle / 2);
	float s = sin(angle / 2);

	Vector3 axisNormal = Vector3Normalize(axis);

	Vector3 v = s * axisNormal;

	Quaternion quaternion = {
		v.x,v.y,v.z,c
	};

	return quaternion;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Vector3 rotated = {};

	Quaternion r = { vector.x,vector.y,vector.z, 0 };

	Quaternion a = QuaternionMultiplication(quaternion, r);

	Quaternion b = QuaternionConjugation(quaternion);

	r = QuaternionMultiplication(a, b);

	rotated = { r.x,r.y,r.z };

	return rotated;
}

Matrix4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4 R = {};

	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	R.m[0][0] = (w * w) + (x * x) - (y * y) - (z * z);
	R.m[0][1] = 2 * ((x * y) + (w * z));
	R.m[0][2] = 2 * ((x * z) - (w * y));
	R.m[0][3] = 0.0f;

	R.m[1][0] = 2 * ((x * y) - (w * z));
	R.m[1][1] = (w * w) - (x * x) + (y * y) - (z * z);
	R.m[1][2] = 2 * ((y * z) + (w * x));
	R.m[1][3] = 0.0f;

	R.m[2][0] = 2 * ((x * z) + (w * y));
	R.m[2][1] = 2 * ((y * z) - (w * x));
	R.m[2][2] = (w * w) - (x * x) - (y * y) + (z * z);
	R.m[2][3] = 0.0f;

	R.m[3][0] = 0.0f;
	R.m[3][1] = 0.0f;
	R.m[3][2] = 0.0f;
	R.m[3][3] = 1.0f;

	return R;
}
