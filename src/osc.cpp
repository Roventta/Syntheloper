#include <../include/stdio.h>
#include <math.h>
#include "../include/osc.h"

Osc::Osc(Syntheloper* sys, float freq, float amp,
	std::string tblname,float(*writer)(double)
	// Ugen's channelGrain has four floars [0]:out, [1]:phase, [2]:freq, [3]:amp;
	) : Ugen(sys->getBus(), 4)
{
	mSystem = sys;
	mWavetable = sys->getWvTble(tblname, writer);
	mSystem->AddUgen(this);
	this->mChannelGrain.Set(1, freq);
	this->mChannelGrain.Set(2, amp);
	this->mChannelGrain.Set(3, 0);
}

//assume samplerate 44100
void Osc::tick(float* buf) {
	if (mWavetable == nullptr) { return; }
	//TODO: figure out which comes first, tick or caculate out
	//and add calculate out here
	float tempF = *this->mChannelGrain.Get(1);
	float tempA = *this->mChannelGrain.Get(2);
	float tempP = *this->mChannelGrain.Get(3);
	float tempO = this->calcCurrentData();
	this->mChannelGrain.Set(0,tempO);
	int tablesize_tmp = mWavetable->getTableSize();
	float phase_increment = (double)tablesize_tmp/44100.0 * tempF;
	tempP += phase_increment;
	if (tempP >= tablesize_tmp) { tempP -= tablesize_tmp;}
	this->mChannelGrain.Set(3, tempP);
	if(Scoped){*buf = tempO; *(buf+1)=tempO;}
	return;
}
