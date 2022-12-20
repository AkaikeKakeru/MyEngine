#pragma once
#include "WinApp.h"
#define DIRECTINPUT_VERSION 0x0800 //DirectInput Version
#include <dinput.h>

class Input {
public://基本の関数
	void Initialize();
	void Update();

private://メンバ変数
	WinApp* winApp_ = nullptr;

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