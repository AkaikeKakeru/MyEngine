#pragma once
#include "DirectXBasis.h"

class ImGuiManager {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize(DirectXBasis* dXBas);
	void Finalize();

	//ImGui受付開始
	void Begin();
	//ImGui受付終了
	void End();
	//画面への描画
	void Draw();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	//コピーコンストラクタを無効
	ImGuiManager(const ImGuiManager&) = delete;
	//代入演算子を無効
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

public:
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;

public://static
	static ImGuiManager* GetInstance();
};