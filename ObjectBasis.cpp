#include "ObjectBasis.h"

void ObjectBasis::Initialize(){
}

void ObjectBasis::PreDraw(){
}

void ObjectBasis::PostDraw(){
}

ObjectBasis* ObjectBasis::GetInstance(){
	static ObjectBasis instace;
	return &instace;
}