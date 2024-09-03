#pragma once

#ifndef QPCGI_H
#define QPCGI_H

#include "QPCGInstruction.fwd.h"
#include "ugen.h"
#include "Syntheloper.fwd.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

class QPCGInstruction {
private:
    float* mSrcLeft;
    float* mSrcRight;
    float* mTarget=nullptr;
	float (*floatOP)(float, float);
	Syntheloper* mSystem;

public:
    float mRegister;
	//only take float assigned memory
	QPCGInstruction(Syntheloper* sys, float* l, float* r, float* t,
	float(*func)(float,float));
	void commune() {
		if (mSrcLeft!=nullptr&&mSrcRight!=nullptr&&floatOP!=nullptr)
		{
		    mRegister = floatOP(*mSrcLeft,*mSrcRight);
			if (mTarget!=nullptr) {
			    *mTarget = mRegister;
			}
 		}
	}
	void setfloatOP(float (*in)(float, float)) { floatOP = in; }
	float* readRegi(){return &mRegister;}
};

#endif
