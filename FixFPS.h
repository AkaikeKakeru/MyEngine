#pragma once
#include <chrono>

class FixFPS {
public: //基本メンバ関数
	void Initialize();
	void Update();

private: //メンバ変数
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;
};