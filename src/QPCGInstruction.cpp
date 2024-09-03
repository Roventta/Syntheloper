#include "../include/QPCGInstruction.h"
#include "../include/Syntheloper.h"

QPCGInstruction::QPCGInstruction(Syntheloper* sys, float* l, float* r, float* t,
float(*func)(float, float)
){
   	this->mSrcLeft = l;
	this->mSrcRight = r;
	if(t!=nullptr){mTarget=t;}
	this->floatOP=func;
	this->mSystem = sys;
	mSystem->AddUgenLink(this);
}
