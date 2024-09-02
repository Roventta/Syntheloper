#pragma once

#ifndef OSC_H
#define OSC_H

#include "Syntheloper.h"
#include "ugen.h"
#include "wavetable.h"
#include "math_synth.h"
#include "QPaudioBus.h"

//[0]:out, [1]:freq, [2]:amp, [3]:phase;

class Osc : protected Ugen {
private:
	Wavetable* mWavetable;
	Syntheloper* mSystem;

public:
    bool Scoped=false;
	Osc(Syntheloper* sys, float freq, float amp,
		std::string tblname, float(*writer)(double));
	~Osc() {};
	void tick(float* buf) override;

	float calcCurrentData() override{
		//float tempF, tempA, tempP;
		float tempF = this->mChannelGrain.Get(1);
		float tempA = this->mChannelGrain.Get(2);
		float tempP = this->mChannelGrain.Get(3);

		float data_floor = mWavetable->readTable((int)floor(tempP));
		float data_ceiling = mWavetable->readTable((int)ceil(tempP));
		double temp;
		float intered_tmp = data_floor + (data_ceiling - data_floor) * (float)modf(tempP, &temp);
		return tempA * intered_tmp;
	}
	void setWavetable(Wavetable* wavetable) { mWavetable = wavetable;};
};

#endif
