#include <../include/stdio.h>
#include <math.h>

#include "../include/osc.h"

Osc::Osc(float freq, float amp) {
	mFrequency = freq; mAmplitude = amp;
	mPhase = 0.0f;

	mSystem = nullptr;
	mWavetable = nullptr;
}

Osc::Osc(Syntheloper* sys, float freq, float amp,
	std::string tblname,float(*writer)(double),
	bool onbus, bool alone)
{
	mFrequency = freq; mAmplitude = amp;
	mPhase = 0.0f;

	mSystem = sys;
	mWavetable = sys->getWvTble(tblname, writer);

	if (onbus) { this->initBusChannel(sys->getBus());}
	if (alone) { mSystem->AddUgen(this); }
}

//assume samplerate 44100
void Osc::tick(float* buf) {
	if (mWavetable == nullptr) { return; }

	float tempF, tempA, tempP;

	if (mChanneled) {
		//struct ChannelGrain *temp = (ChannelGrain*)mBusChannel;
		readFromBus(&tempF, mCGPointers[1], sizeof(float));
		readFromBus(&tempA, mCGPointers[2], sizeof(float));
		readFromBus(&tempP, mCGPointers[3], sizeof(float));
	}
	else {
		tempF = mFrequency;
		tempA = mAmplitude;
		tempP = mPhase;
	}

	int tablesize_tmp = mWavetable->getTableSize();
	float phase_increment = (double)tablesize_tmp / 44100.0 * tempF;
	tempP += phase_increment;
	//int phaseRound = (int)round(mPhase);
	if (tempP >= tablesize_tmp) { tempP -= tablesize_tmp; }
	float out = readCurrentData();
	if (mChanneled) {
		struct ChannelGrain* temp = (ChannelGrain*)mBusChannel;
		writeToBus(mCGPointers[3], &tempP, sizeof(float));
		writeToBus(mCGPointers[0], &out, sizeof(float));
	}
	else
	{
		mPhase = tempP;
	}

	if (mScoped) { *buf = out; *(buf + 1) = out;
	}

	return;
}
