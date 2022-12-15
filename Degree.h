#pragma once

float ConvertToRadian(float degree) {
	const float PI = 3.14159265;

	float radian = 0;
	radian = degree * (3.14 / 180);

	return radian;
}