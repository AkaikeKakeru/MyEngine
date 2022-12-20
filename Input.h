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

public: //public固有関数
	/// <summary>
	/// キーを押し続けているか
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>押し続けているか否か</returns>
	bool PressKey(BYTE keyNum);

	/// <summary>
	/// キーを今押した瞬間か
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>今押した瞬間か否か</returns>
	bool TriggerKey(BYTE keyNum);

	/// <summary>
	/// キーを離した瞬間か
	/// </summary>
	/// <param name="keyNum">キー番号</param>
	/// <returns>離した瞬間か否か</returns>
	bool ReleaseKey(BYTE keyNum);

public://固有関数
	void GenerateDirectInput();
	void GenerateKeyBoardDevice();

private://メンバ変数
	//Windowsアプリケーション
	WinApp* winApp_ = nullptr;

	//DirectInput
	ComPtr<IDirectInput8> dInput_;
	//キーボード情報
	ComPtr<IDirectInputDevice8> keyboard_;

	//押し続けられているかのキー情報保存配列
	BYTE key_[256] = {};
	//今押された瞬間かの確認用キー情報保存配列
	BYTE keyPre_[256] = {};

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