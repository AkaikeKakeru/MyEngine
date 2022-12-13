#pragma once
#include "DirectXBasis.h"

class DrawBasis {
public:
	void Initialize();
private:
	DirectXBasis* dxBas_ = nullptr;
};