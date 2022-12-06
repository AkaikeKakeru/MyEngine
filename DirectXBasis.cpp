#include "DirectXBasis.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <vector>
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

void DirectXBasis::Initialize(){
	InitDevice();
	InitCommand();
	InitSwapChain();
	InitRenderTargetView();
	InitDepthBuffer();
	InitFence();
}

void DirectXBasis::InitDevice(){
}

void DirectXBasis::InitCommand(){
}

void DirectXBasis::InitSwapChain(){
}

void DirectXBasis::InitRenderTargetView(){
}

void DirectXBasis::InitDepthBuffer(){
}

void DirectXBasis::InitFence(){
}

void DirectXBasis::Draw(){
}