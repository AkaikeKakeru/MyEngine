#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize() {
	HRESULT result;
	winApp_ = WinApp::GetInstance();

	//DirectInput初期化
	IDirectInput8* dInput = nullptr;
	result = DirectInput8Create(
		winApp_->GetHInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dInput, nullptr);
	assert(SUCCEEDED(result));
}

void Input::Update() {
}

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}