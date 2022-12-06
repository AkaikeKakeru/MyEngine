#pragma once

class DirectXBasis {
public://static
public://基本関数	
	void Initialize();
	void Draw();

public://固有関数
	///初期化
	//デバイス
	void InitDevice();
	//コマンド
	void InitCommand();
	//スワップチェーン
	void InitSwapChain();
	//レンダ―ターゲットビュー
	void InitRenderTargetView();
	//深度バッファ
	void InitDepthBuffer();
	//フェンス
	void InitFence();

public://定数
private://変数
};