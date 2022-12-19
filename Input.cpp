#include "Input.h"

void Input::Initialize() {
}

void Input::Update() {
}

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}