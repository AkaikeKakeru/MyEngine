#pragma once
#include "WinApp.h"
#define DIRECTINPUT_VERSION 0x0800 //DirectInput Version
#include <dinput.h>
#include <wrl.h>

class Input {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public://基本の関数
	void Initialize();
	void Update();

public://固有関数
	void GenerateDirectInput();
	void GenerateKeyBoardDevice();


private://メンバ変数
	WinApp* winApp_ = nullptr;

	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	ComPtr<IDirectInput8> dInput_ = nullptr;

private:
	Input() = default;
	~Input() = default;
	//コピーコンストラクタを無効
	Input(const Input&) = delete;
	//代入演算子を無効
	const Input& operator=(const Input&) = delete;

public://static
	static Input* GetInstance();
};