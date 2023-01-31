#pragma once
#include "DirectXBasis.h"

class ImGuiManager {
public:
	void Initialize(DirectXBasis* dXBas);
private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	//コピーコンストラクタを無効
	ImGuiManager(const ImGuiManager&) = delete;
	//代入演算子を無効
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

public://static
	static ImGuiManager* GetInstance();
};