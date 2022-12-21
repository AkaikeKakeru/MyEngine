#pragma once

class ObjectBasis {
private://省略
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private://構造体

public://基本関数
	void Initialize();
	void PreDraw();
	void PostDraw();

private://メンバ変数

private:
	ObjectBasis() = default;
	~ObjectBasis() = default;
	//コピーコンストラクタを無効
	ObjectBasis(const ObjectBasis&) = delete;
	//代入演算子を無効
	const ObjectBasis& operator=(const ObjectBasis&) = delete;

public://static
	static ObjectBasis* GetInstance();
};