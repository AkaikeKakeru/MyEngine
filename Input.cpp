#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize() {
	winApp_ = WinApp::GetInstance();

	GenerateDirectInput();
	GenerateKeyBoardDevice();
}

void Input::Update() {
}

void Input::GenerateDirectInput() {
	HRESULT result;

	//DirectInput初期化
	result = DirectInput8Create(
		winApp_->GetHInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dInput_, nullptr);
	assert(SUCCEEDED(result));
}

void Input::GenerateKeyBoardDevice() {
	HRESULT result;

	//キーボードデバイス生成
	result = dInput_->CreateDevice(
		GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(
		&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		winApp_->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}