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
	//key_配列を上書きする前に、内容をkeyPre_にコピー
	memcpy(keyPre_, key_, sizeof(key_));

	//キーボード情報取得開始
	keyboard_->Acquire();
	//全キーの入力情報を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool Input::PressKey(BYTE keyNum) {
	//指定番号のキーが、押されているなら
	if (key_[keyNum]) {
		return true;
	}
	//それ以外はfalse
	return false;
}

bool Input::TriggerKey(BYTE keyNum) {
	//指定番号のキーが↓
	//1F前の時点で、押されていないなら
	if (!keyPre_[keyNum]) {
		//今は押されているなら
		if (key_[keyNum]) {
			return true;
		}
	}
	//それ以外はfalse
	return false;
}

bool Input::ReleaseKey(BYTE keyNum) {
	//指定番号のキーが↓
	//1F前の時点で、押されているなら
	if (keyPre_[keyNum]) {
		//今は押されていないなら
		if (!key_[keyNum]) {
			return true;
		}
	}
	//それ以外はfalse
	return false;
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